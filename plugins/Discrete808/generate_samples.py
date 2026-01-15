
import math
import random
import argparse
import struct
import wave

# --- DSP Utilities ---

class AnalogKit:
    class OpAmpModel:
        kIdeal = 0
        kuPC4558 = 1
        kTL072 = 2
        kBroken = 3

    class DiodeModel:
        kSilicon = 0
        kGermanium = 1
        kLED = 2

    class CircuitConfig:
        def __init__(self):
            self.opAmp = AnalogKit.OpAmpModel.kuPC4558
            self.diode = AnalogKit.DiodeModel.kSilicon
            self.noiseColor = 0.0

    @staticmethod
    def diodeSaturate(x, model):
        if model == AnalogKit.DiodeModel.kSilicon:
            threshold = 0.7
            return math.tanh(x / threshold) * threshold
        elif model == AnalogKit.DiodeModel.kGermanium:
            threshold = 0.3
            return math.tanh(x * 1.5) * threshold
        elif model == AnalogKit.DiodeModel.kLED:
            threshold = 1.8
            return math.tanh(x / threshold) * threshold
        return math.tanh(x)

    class SlewLimiter:
        def __init__(self):
            self.lastOutput = 0.0
            self.sampleRate = 44100.0

        def prepare(self, sr):
            self.sampleRate = sr
            self.reset()
        
        def reset(self):
            self.lastOutput = 0.0

        def process(self, input_val, v_per_us):
            maxDelta = (v_per_us * 1.0e6) / self.sampleRate
            delta = input_val - self.lastOutput
            if delta > maxDelta:
                delta = maxDelta
            elif delta < -maxDelta:
                delta = -maxDelta
            
            self.lastOutput += delta
            return self.lastOutput

    class OnePoleLPF:
        def __init__(self):
            self.z1 = 0.0
        
        def reset(self):
            self.z1 = 0.0
            
        def process(self, x, a0):
            self.z1 = x * a0 + self.z1 * (1.0 - a0)
            return self.z1
    
    class NoiseSource:
        def __init__(self):
            self.lastOutput = 0.0
            
        def getNextSample(self):
            white = random.uniform(-1.0, 1.0)
            # 12kHz LPF approx
            self.lastOutput = 0.7 * white + 0.3 * self.lastOutput
            return self.lastOutput

class ZDFResonator:
    def __init__(self):
        self.s1 = 0.0
        self.s2 = 0.0
        self.a1 = 0.0
        self.a2 = 0.0
        self.a3 = 0.0
        self.sampleRate = 44100.0
    
    def reset(self):
        self.s1 = 0.0
        self.s2 = 0.0

    def setParameters(self, sampleRate, cutoff, resonance):
        if cutoff <= 0.0: cutoff = 20.0
        if resonance <= 0.001: resonance = 0.001
        
        g = math.tan(math.pi * cutoff / sampleRate)
        k = 1.0 / resonance
        denominator = 1.0 / (1.0 + g * (g + k))
        
        self.a1 = denominator
        self.a2 = g * self.a1
        self.a3 = g * self.a2
    
    def process(self, input_val):
        v3 = input_val - self.s2
        v1 = self.a1 * self.s1 + self.a2 * v3
        v2 = self.s2 + self.a2 * self.s1 + self.a3 * v3
        
        self.s1 = 2.0 * v1 - self.s1
        self.s2 = 2.0 * v2 - self.s2
        return v2

# --- Voices ---

class KickVoice:
    def __init__(self, sampleRate=44100.0):
        self.sampleRate = sampleRate
        self.resonator = ZDFResonator()
        self.slewLimiter = AnalogKit.SlewLimiter()
        self.toneLPF = AnalogKit.OnePoleLPF()
        self.circuitConfig = AnalogKit.CircuitConfig()
        
        # Params
        self.tuneParam = 0.5
        self.decayParam = 0.5
        self.toneParam = 0.5
        self.levelParam = 0.8
        self.velToTone = 0.0
        self.velToDecay = 0.0
        
        # State
        self.pulseTimer = 0
        self.pitchEnvelope = 0.0
        self.isTriggered = False
        self.velocity = 0.0
        
        self.prepare(sampleRate)

    def prepare(self, sr):
        self.sampleRate = sr
        self.resonator.setParameters(sr, 50.0, 10.0)
        self.slewLimiter.prepare(sr)
        self.toneLPF.reset()

    def trigger(self, velocity):
        self.velocity = velocity
        self.isTriggered = True
        self.pulseTimer = int(0.001 * self.sampleRate)
        self.pitchEnvelope = 1.0

    def renderSample(self, shared_noise=0.0):
        if not self.isTriggered:
            return 0.0
            
        # 1. Trigger
        input_val = 0.0
        if self.pulseTimer > 0:
            input_val = 1.0 * self.velocity
            self.pulseTimer -= 1
            
        # 2. Mod
        modTone = max(0.0, min(1.0, self.toneParam + (self.velocity - 0.5) * self.velToTone))
        modDecay = max(0.0, min(1.0, self.decayParam + (self.velocity - 0.5) * self.velToDecay))
        
        # Accent (simplified)
        currentVelocity = self.velocity
        
        # 3. Pitch Env
        baseFreq = 40.0 + (self.tuneParam * 40.0)
        pitchSweep = 80.0 * self.pitchEnvelope
        currentFreq = baseFreq + pitchSweep
        
        self.pitchEnvelope *= 0.9994
        if self.pitchEnvelope < 0.0001:
            self.pitchEnvelope = 0.0
            self.isTriggered = False
            
        # 4. Resonator
        Q = 2.0 + (modDecay * modDecay * 80.0)
        self.resonator.setParameters(self.sampleRate, currentFreq, Q)
        rawBody = self.resonator.process(input_val * currentVelocity)
        
        # 5. Tone
        a0 = 0.05 + (modTone * 0.4)
        filtered = self.toneLPF.process(rawBody, a0)
        
        # 6. Slew & Saturation
        slewRate = 1.0
        if self.circuitConfig.opAmp == AnalogKit.OpAmpModel.kTL072:
            slewRate = 13.0
        elif self.circuitConfig.opAmp == AnalogKit.OpAmpModel.kuPC4558:
            slewRate = 1.0
            
        slewed = self.slewLimiter.process(filtered, slewRate)
        output = AnalogKit.diodeSaturate(slewed * 4.0, self.circuitConfig.diode)
        
        return output * self.levelParam

class SnareVoice:
    def __init__(self, sampleRate=44100.0):
        self.sampleRate = sampleRate
        self.resonatorHigh = ZDFResonator()
        self.resonatorLow = ZDFResonator()
        self.slewLimiter = AnalogKit.SlewLimiter()
        self.toneLPF = AnalogKit.OnePoleLPF()
        self.circuitConfig = AnalogKit.CircuitConfig()
        
        # Params
        self.tuneParam = 0.5
        self.decayParam = 0.5
        self.toneParam = 0.5
        self.snappyParam = 0.5
        self.levelParam = 0.8
        self.velToTone = 0.0
        self.velToDecay = 0.0
        
        # State
        self.pulseTimer = 0
        self.snappyEnvelope = 0.0
        self.noiseHPF = 0.0
        self.isTriggered = False
        self.velocity = 0.0
        
        self.prepare(sampleRate)

    def prepare(self, sr):
        self.sampleRate = sr
        self.resonatorHigh.setParameters(sr, 330.0, 15.0)
        self.resonatorLow.setParameters(sr, 180.0, 15.0)
        self.slewLimiter.prepare(sr)
        self.toneLPF.reset()

    def trigger(self, velocity):
        self.velocity = velocity
        self.isTriggered = True
        self.pulseTimer = int(0.001 * self.sampleRate)
        self.snappyEnvelope = 1.0

    def renderSample(self, shared_noise):
        if not self.isTriggered and self.snappyEnvelope <= 0.0:
            return 0.0
            
        # 1. Trigger
        input_val = 0.0
        if self.pulseTimer > 0:
            input_val = 1.0 * self.velocity
            self.pulseTimer -= 1
            
        # 2. Mod
        modSnappy = max(0.0, min(1.0, self.snappyParam * (1.0 + (self.velocity - 0.5) * self.velToTone)))
        modDecay = max(0.0, min(1.0, self.toneParam * (1.0 + (self.velocity - 0.5) * self.velToDecay)))
        
        # Resonators
        q = 10.0 + (modDecay * 10.0)
        self.resonatorHigh.setParameters(self.sampleRate, 330.0, q)
        self.resonatorLow.setParameters(self.sampleRate, 180.0, q)
        
        body1 = self.resonatorHigh.process(input_val)
        body2 = self.resonatorLow.process(input_val)
        bodySum = (body1 + body2) * 0.5
        
        # Snappy
        self.snappyEnvelope *= 0.9992
        if self.snappyEnvelope < 0.0001:
            self.snappyEnvelope = 0.0
            self.isTriggered = False
            
        hpfAmount = 0.9 + (self.circuitConfig.noiseColor * 0.08)
        self.noiseHPF = shared_noise * (1.0 - hpfAmount) + self.noiseHPF * hpfAmount
        highPassedNoise = shared_noise - self.noiseHPF
        
        snappySig = highPassedNoise * self.snappyEnvelope * modSnappy
        
        mixed = (bodySum * 0.4) + (snappySig * 0.6)
        
        output = AnalogKit.diodeSaturate(mixed * (2.0 + self.velocity), self.circuitConfig.diode)
        return output * self.levelParam

# --- File Utils ---

def save_wav(filename, samples, sample_rate=44100):
    # Normalize to -3dB
    max_val = max(abs(s) for s in samples)
    if max_val > 0:
        samples = [s / max_val * 0.707 for s in samples]
        
    # Convert to 16-bit PCM
    data = bytearray()
    for s in samples:
        s = max(-1.0, min(1.0, s))
        val = int(s * 32767.0)
        data.extend(struct.pack('<h', val))
        
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        f.writeframes(data)
    print(f"Saved {filename}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate Discrete808 samples")
    parser.add_argument("--kick_tune", type=float, default=0.5, help="Kick Tune (0-1)")
    parser.add_argument("--kick_decay", type=float, default=0.5, help="Kick Decay (0-1)")
    parser.add_argument("--kick_tone", type=float, default=0.5, help="Kick Tone (0-1)")
    
    parser.add_argument("--snare_tune", type=float, default=0.5, help="Snare Tune (0-1)")
    parser.add_argument("--snare_snap", type=float, default=0.5, help="Snare Snappy (0-1)")
    
    args = parser.parse_args()
    
    SR = 44100
    
    # 1. Generate Kick
    kick = KickVoice(SR)
    kick.tuneParam = args.kick_tune
    kick.decayParam = args.kick_decay
    kick.toneParam = args.kick_tone
    
    kick_samples = []
    kick.trigger(0.9)
    noise_source = AnalogKit.NoiseSource()
    
    for _ in range(int(SR * 1.0)): # 1 second
        noise = noise_source.getNextSample()
        s = kick.renderSample(noise)
        kick_samples.append(s)
        
    save_wav("preview_kick.wav", kick_samples, SR)
    
    # 2. Generate Snare
    snare = SnareVoice(SR)
    snare.tuneParam = args.snare_tune
    snare.snappyParam = args.snare_snap
    
    snare_samples = []
    snare.trigger(0.9)
    
    for _ in range(int(SR * 1.0)): # 1 second
        noise = noise_source.getNextSample()
        s = snare.renderSample(noise)
        snare_samples.append(s)
        
    save_wav("preview_snare.wav", snare_samples, SR)
