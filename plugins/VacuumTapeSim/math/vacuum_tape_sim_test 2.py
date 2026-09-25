import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import os

def langevin(x):
    # Avoid division by zero by using Taylor approximation near 0
    return np.where(np.abs(x) < 1e-4, x / 3.0, 1.0 / np.tanh(x + 1e-9) - 1.0 / (x + 1e-9))


# Create output directory for graphs
os.makedirs("media/tape_sim", exist_ok=True)

fs = 48000.0

def process_voltage_sag(x, threshold=0.1, tau_drain=0.01, tau_recovery=0.1, tau_env=0.005):
    V = np.ones_like(x)
    y = np.zeros_like(x)
    
    alpha = 1.0 - np.exp(-1.0 / (tau_drain * fs))
    beta = 1.0 - np.exp(-1.0 / (tau_recovery * fs))
    alpha_env = 1.0 - np.exp(-1.0 / (tau_env * fs))
    
    v_state = 1.0
    env_state = 0.0
    for n in range(len(x)):
        # One-pole envelope follower
        env_state += alpha_env * (abs(x[n]) - env_state)
        
        # Drain if over threshold
        drain_amt = max(0.0, env_state - threshold)
        
        v_state = v_state - (alpha * drain_amt) + (beta * (1.0 - v_state))
        
        # Clamp V between 0 and 1
        v_state = max(0.0, min(1.0, v_state))
        
        V[n] = v_state
        y[n] = x[n] * v_state
        
    return y, V

def process_hysteresis(x, drive=2.0, coercivity=0.5, beta=0.002, x_bias=0.1):
    y = np.zeros_like(x)
    x_prev = 0.0
    
    for n in range(len(x)):
        dx = (x[n] - x_prev) * fs
        y[n] = langevin(drive * (x[n] + x_bias) - coercivity * langevin(beta * dx))
        x_prev = x[n]
        
    return y

# --- TEST 1: Voltage Sag ---
# Generate a burst signal
t = np.arange(fs) / fs
burst = np.sin(2 * np.pi * 100 * t)
burst[int(0.2*fs):int(0.4*fs)] *= 2.0  # High energy burst
burst[int(0.6*fs):int(0.8*fs)] *= 1.5

y_sag, v_env = process_voltage_sag(burst, threshold=0.5, tau_drain=0.01, tau_recovery=0.1)

plt.figure(figsize=(10, 6))
plt.plot(t, burst, label="Input Signal", alpha=0.5, color='gray')
plt.plot(t, y_sag, label="Output Signal (Sagged)", alpha=0.8, color='orange')
plt.plot(t, v_env, label="Voltage State (V[n])", color='red', linewidth=2)
plt.title("Voltage Sag Compression")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude / Voltage")
plt.legend()
plt.tight_layout()
plt.savefig("media/tape_sim/voltage_sag.png")
plt.close()

# --- TEST 2: Magnetic Hysteresis Loop ---
# Generate a simple 500Hz sine wave
t_sine = np.arange(int(fs*0.01)) / fs
x_sine = np.sin(2 * np.pi * 500 * t_sine)

# Apply hysteresis
y_hyst = process_hysteresis(x_sine, drive=2.5, coercivity=0.8, beta=0.002)

plt.figure(figsize=(8, 8))
plt.plot(x_sine, y_hyst, color='magenta', linewidth=2)
plt.title("Magnetic Hysteresis Curve (Phase Plot)")
plt.xlabel("Input x[n]")
plt.ylabel("Magnetization y[n]")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("media/tape_sim/hysteresis_loop.png")
plt.close()

# --- TEST 3: Harmonic Distortion ---
# Process a 1kHz sine wave and plot FFT
x_1k = np.sin(2 * np.pi * 1000 * np.arange(fs) / fs)
y_1k = process_hysteresis(x_1k, drive=3.0, coercivity=0.6, beta=0.002)

# Window and FFT
window = signal.windows.hann(len(x_1k))
X_fft = np.fft.rfft(x_1k * window)
Y_fft = np.fft.rfft(y_1k * window)
freqs = np.fft.rfftfreq(len(x_1k), 1/fs)

X_mag = 20 * np.log10(np.abs(X_fft) + 1e-9)
Y_mag = 20 * np.log10(np.abs(Y_fft) + 1e-9)

plt.figure(figsize=(10, 6))
plt.plot(freqs, X_mag, label="Input 1kHz", alpha=0.7, color='cyan')
plt.plot(freqs, Y_mag, label="Tape Output", alpha=0.8, color='magenta')
plt.title("Tape Hysteresis Harmonic Spectrum")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude (dB)")
plt.xlim(0, 10000)
plt.ylim(0, max(Y_mag) + 10)
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("media/tape_sim/harmonic_spectrum.png")
plt.close()

print("Graphs generated successfully.")
