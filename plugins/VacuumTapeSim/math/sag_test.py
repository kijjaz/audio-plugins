import numpy as np
import matplotlib.pyplot as plt

# Parameters
fs = 44100
duration = 1.0
t = np.linspace(0, duration, int(fs * duration), endpoint=False)

# 1. Generate Test Signals
# Signal A: A transient snare drum hit at 0.1s
snare = np.zeros_like(t)
snare[int(0.1*fs):int(0.15*fs)] = np.random.randn(int(0.05*fs)) * np.exp(-np.linspace(0, 5, int(0.05*fs)))
snare *= 0.8 # Peak at 0.8 (approx +2 VU if 0.775 is 0 VU)

# Signal B: A continuous loud bass note from 0.4s to 0.7s
bass = np.zeros_like(t)
bass[int(0.4*fs):int(0.7*fs)] = np.sin(2 * np.pi * 50 * t[int(0.4*fs):int(0.7*fs)])
bass *= 1.2 # Pushed hard, +4 VU

# Combine
x = snare + bass

# 2. Voltage Sag Algorithm
# From our spec:
T = 0.5         # Threshold
tau_env = 0.005 # 5ms Envelope Smoothing
tau_drain = 0.05 # 50ms Drain
tau_recovery = 0.2 # 200ms Recovery

alpha_env = 1.0 - np.exp(-1.0 / (tau_env * fs))
alpha_drain = 1.0 - np.exp(-1.0 / (tau_drain * fs))
beta_recovery = 1.0 - np.exp(-1.0 / (tau_recovery * fs))

V = np.ones_like(x)
env = np.zeros_like(x)

v_state = 1.0
env_state = 0.0

for n in range(len(x)):
    # Envelope Follower
    env_state = env_state + alpha_env * (abs(x[n]) - env_state)
    env[n] = env_state
    
    # Sag logic
    if env_state > T:
        v_state = v_state - alpha_drain * (env_state - T)
    else:
        v_state = v_state + beta_recovery * (1.0 - v_state)
        
    # Clamp
    if v_state < 0.1: v_state = 0.1
    if v_state > 1.0: v_state = 1.0
    
    V[n] = v_state

# 3. Plotting
plt.figure(figsize=(10, 6))

plt.subplot(2, 1, 1)
plt.plot(t, x, label='Input Audio', alpha=0.5, color='gray')
plt.plot(t, env, label='Envelope (env_state)', color='blue', linewidth=2)
plt.axhline(T, color='red', linestyle='--', label=f'Threshold (T={T})')
plt.title('Audio Input vs Threshold (VU concept)')
plt.ylabel('Amplitude')
plt.legend(loc='upper right')
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(t, V, label='Voltage State (V[n])', color='orange', linewidth=3)
plt.ylim(0, 1.1)
plt.title('Vacuum Tube Power Supply (Voltage Sag)')
plt.xlabel('Time (s)')
plt.ylabel('Available Voltage (0.0 - 1.0)')
plt.legend(loc='upper right')
plt.grid(True)

plt.tight_layout()
plt.savefig('sag_test.png', dpi=150)
print("Saved sag_test.png")
