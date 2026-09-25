import numpy as np
import matplotlib.pyplot as plt

# Parameters
fs = 44100
duration = 1.0
t = np.linspace(0, duration, int(fs * duration), endpoint=False)

# 1. Generate Test Signals
# Signal A: Transient snare (0.1s)
snare = np.zeros_like(t)
snare[int(0.1*fs):int(0.15*fs)] = np.random.randn(int(0.05*fs)) * np.exp(-np.linspace(0, 5, int(0.05*fs)))
snare *= 0.8

# Signal B: Continuous loud bass note (0.4s to 0.7s) at low freq (20Hz) to test ripple
bass = np.zeros_like(t)
bass[int(0.4*fs):int(0.7*fs)] = np.sin(2 * np.pi * 20 * t[int(0.4*fs):int(0.7*fs)])
bass *= 1.2

x = snare + bass

# 2. Advanced Voltage Sag Algorithm
T = 0.5

# Time Constants
tau_env = 0.005 # 5ms
tau_drain = 0.01 # Faster drain multiplier since it's soft knee
tau_fast = 0.05 # 50ms fast recovery
tau_slow = 0.5  # 500ms slow recovery
tau_slow_env = 0.2 # 200ms integration for program dependence

alpha_env = 1.0 - np.exp(-1.0 / (tau_env * fs))
alpha_drain = 1.0 - np.exp(-1.0 / (tau_drain * fs))
alpha_slow_env = 1.0 - np.exp(-1.0 / (tau_slow_env * fs))

beta_fast = 1.0 - np.exp(-1.0 / (tau_fast * fs))
beta_slow = 1.0 - np.exp(-1.0 / (tau_slow * fs))

V = np.ones_like(x)
env_rms = np.zeros_like(x)
env_slow = np.zeros_like(x)

v_state = 1.0
e1 = 0.0
e_rms = 0.0
e_slow = 0.0

for n in range(len(x)):
    # 1. True RMS (2-pole filter)
    x_sq = x[n]**2
    e1 = e1 + alpha_env * (x_sq - e1)
    e_rms = e_rms + alpha_env * (e1 - e_rms)
    
    # 2. Slow Envelope for Program-Dependent Recovery
    e_slow = e_slow + alpha_slow_env * (e_rms - e_slow)
    
    # Square root to get linear amplitude comparison
    e_lin = np.sqrt(max(0, e_rms))
    env_rms[n] = e_lin
    env_slow[n] = e_slow
    
    # 3. Soft-Knee Drain
    if e_lin > T:
        drain = alpha_drain * ((e_lin - T)**2)
    else:
        drain = 0.0
        
    # 4. Dual-Stage Recovery
    # Limit e_slow to 0-1 range for interpolation
    blend = min(1.0, e_slow * 2.0)
    beta_dynamic = beta_fast * (1.0 - blend) + beta_slow * blend
    
    # 5. Voltage Update
    v_state = v_state - drain + beta_dynamic * (1.0 - v_state)
    v_state = max(0.1, min(1.0, v_state))
    
    V[n] = v_state

# 3. Plotting
plt.figure(figsize=(10, 6))

plt.subplot(2, 1, 1)
plt.plot(t, x, label='Input Audio (20Hz Bass)', alpha=0.5, color='gray')
plt.plot(t, env_rms, label='2-Pole RMS Envelope', color='blue', linewidth=2)
plt.axhline(T, color='red', linestyle='--', label=f'Threshold (T={T})')
plt.title('RMS Envelope (Ripple Rejection)')
plt.ylabel('Amplitude')
plt.legend(loc='upper right')
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(t, V, label='Voltage State (Soft Knee + Dual Stage)', color='orange', linewidth=3)
plt.ylim(0, 1.1)
plt.title('Advanced Vacuum Tube Power Supply')
plt.xlabel('Time (s)')
plt.ylabel('Available Voltage (0.1 - 1.0)')
plt.legend(loc='lower right')
plt.grid(True)

plt.tight_layout()
plt.savefig('sag_test_advanced.png', dpi=150)
print("Saved sag_test_advanced.png")
