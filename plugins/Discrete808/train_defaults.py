
import wave
import struct
import math
import random
import os
import sys

# Import our DSP models
from generate_samples import KickVoice, SnareVoice, AnalogKit

def load_wav(path):
    try:
        with wave.open(path, 'rb') as wf:
            n_channels = wf.getnchannels()
            width = wf.getsampwidth()
            frames = wf.readframes(wf.getnframes())
            
            # Convert to floats
            # Assume 16-bit or 24-bit PCM. Most samples are 24-bit or 16-bit.
            # Struct unpacking usually simpler for 16-bit.
            
            samples = []
            total_samples = len(frames) // width
            
            for i in range(0, len(frames), width * n_channels):
                # Just take first channel (left)
                frame_data = frames[i:i+width]
                
                val = 0
                if width == 2:
                    val = struct.unpack('<h', frame_data)[0]
                    val /= 32768.0
                elif width == 3:
                     # 24-bit is tricky in struct, manual unpack
                     # Little endian
                     b0 = frame_data[0]
                     b1 = frame_data[1]
                     b2 = frame_data[2]
                     val = (b2 << 16) | (b1 << 8) | b0
                     if val & 0x800000:
                         val -= 0x1000000
                     val /= 8388608.0
                elif width == 1:
                     val = (frame_data[0] - 128) / 128.0
                
                samples.append(val)
                
            return float(wf.getframerate()), samples
    except Exception as e:
        print(f"Error loading {path}: {e}")
        return 44100.0, []

def get_envelope(signal, window_size=500):
    # Simple moving average of abs value
    env = []
    current_sum = 0.0
    for i in range(len(signal)):
        val = abs(signal[i])
        if i < window_size:
            current_sum += val
            env.append(current_sum / (i + 1))
        else:
            current_sum += val
            current_sum -= abs(signal[i - window_size])
            env.append(current_sum / window_size)
    return env

def compute_loss(target, candidate):
    # 1. Normalize
    max_t = max(abs(x) for x in target) if target else 1.0
    if max_t > 0: target = [x / max_t for x in target]
    
    max_c = max(abs(x) for x in candidate) if candidate else 1.0
    if max_c > 0: candidate = [x / max_c for x in candidate]
    
    # Trim to shortest
    length = min(len(target), len(candidate))
    target = target[:length]
    candidate = candidate[:length]
    
    # 2. Envelope MSE
    # Matches shape and decay
    env_t = get_envelope(target)
    env_c = get_envelope(candidate)
    
    mse = sum((t - c) ** 2 for t, c in zip(env_t, env_c)) / length
    return mse

def hill_climb_kick(target_path, steps=50):
    print(f"Optimizing Kick: {os.path.basename(target_path)}")
    sr_target, target_data = load_wav(target_path)
    if not target_data: return [0.5, 0.5, 0.5]
    
    SR = 44100
    # Limit analysis length for speed (0.3s is enough for kick body/tone)
    analysis_len = int(0.3 * SR)
    target_data = target_data[:analysis_len]

    # Params: [Tune, Decay, Tone]
    best_params = [0.5, 0.5, 0.5]
    best_loss = float('inf')
    
    noise_source = AnalogKit.NoiseSource()

    def get_candidate_loss(params):
        kick = KickVoice(SR)
        kick.tuneParam = params[0]
        kick.decayParam = params[1]
        kick.toneParam = params[2]
        kick.levelParam = 1.0
        
        kick.trigger(1.0)
        
        samples = []
        # Pre-fill noise to keep deterministic sequence if we reset seed? 
        # Actually standard 808 noise is random white. 
        # Using same seed for every run is better for optimization stability.
        random.seed(42) 
        
        for _ in range(analysis_len):
            # Simple white noise gen
            n = random.uniform(-1.0, 1.0) 
            s = kick.renderSample(n)
            samples.append(s)
            
        return compute_loss(target_data, samples)

    # Initial loss
    best_loss = get_candidate_loss(best_params)
    print(f"Initial Loss: {best_loss:.6f}")
    
    step_size = 0.1
    
    for i in range(steps):
        # Try a random neighbor
        candidate = list(best_params)
        idx = random.randint(0, 2)
        change = random.uniform(-step_size, step_size)
        candidate[idx] = max(0.0, min(1.0, candidate[idx] + change))
        
        loss = get_candidate_loss(candidate)
        
        if loss < best_loss:
            best_loss = loss
            best_params = candidate
            print(f"Iter {i}: Found better {best_params} (Loss: {best_loss:.6f})")
            # Increase exploration if stuck? No, just keep going or reduce step
        else:
            # Reduce step size slowly to converge
            step_size *= 0.99
            
    return best_params

def hill_climb_snare(target_path, steps=50):
    print(f"Optimizing Snare: {os.path.basename(target_path)}")
    sr_target, target_data = load_wav(target_path)
    if not target_data: return [0.5, 0.5, 0.5]
    
    SR = 44100
    analysis_len = int(0.2 * SR)
    target_data = target_data[:analysis_len]
    
    # Params: [Tune, Snappy, Tone]
    best_params = [0.5, 0.5, 0.5]
    best_loss = float('inf')

    def get_candidate_loss(params):
        snare = SnareVoice(SR)
        snare.tuneParam = params[0]
        snare.snappyParam = params[1]
        snare.toneParam = params[2]
        snare.decayParam = 0.5 
        
        snare.trigger(1.0)
        
        samples = []
        random.seed(42)
        for _ in range(analysis_len):
            n = random.uniform(-1.0, 1.0)
            s = snare.renderSample(n)
            samples.append(s)
            
        return compute_loss(target_data, samples)

    best_loss = get_candidate_loss(best_params)
    print(f"Initial Loss: {best_loss:.6f}")
    
    step_size = 0.1
    
    for i in range(steps):
        candidate = list(best_params)
        idx = random.randint(0, 2)
        change = random.uniform(-step_size, step_size)
        candidate[idx] = max(0.0, min(1.0, candidate[idx] + change))
        
        loss = get_candidate_loss(candidate)
        
        if loss < best_loss:
            best_loss = loss
            best_params = candidate
            print(f"Iter {i}: Found better {best_params} (Loss: {best_loss:.6f})")
        else:
            step_size *= 0.99
            
    return best_params

if __name__ == "__main__":
    base_path = "/Users/kijjaz/Desktop/Antigravity/2026/20260105 WAM/plugins/Discrete808/BPB Cassette 808/Samples/Clean Kit/" 
    
    print("--- STARTING OPTIMIZATION ---")
    
    kick_best = hill_climb_kick(base_path + "kick01rr1.wav", steps=100)
    print(f"FINAL KICK: Tune={kick_best[0]:.3f}, Decay={kick_best[1]:.3f}, Tone={kick_best[2]:.3f}")
    
    snare_best = hill_climb_snare(base_path + "snare01.wav", steps=100)
    print(f"FINAL SNARE: Tune={snare_best[0]:.3f}, Snappy={snare_best[1]:.3f}, Tone={snare_best[2]:.3f}")
