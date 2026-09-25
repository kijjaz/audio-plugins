import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter, iirpeak, freqz, iirfilter

fs = 44100

def langevin(x):
    # Taylor approximation near 0
    mask = np.abs(x) < 1e-4
    out = np.zeros_like(x)
    out[mask] = x[mask] / 3.0
    out[~mask] = 1.0 / np.tanh(x[~mask]) - 1.0 / x[~mask]
    return out

def process_tape(x, drive=1.0, bias=0.0, asym=0.0, ips=15.0, eq_mode='NAB'):
    # 1. Setup Parameters
    beta_hyst = 1.0 * (2.0**bias)
    fc = min(20000, max(20, (ips * 650.0) * (2.0**(-bias/2.0))))
    fbump = ips * 3.0
    
    # 2. Pre-Emphasis
    # simplified high shelf
    # we'll use a simple 1-pole high-pass mixed with dry for a shelf for the python test
    # (Actually scipy has no easy direct iir high shelf, let's skip strict EQ for the harmonics test,
    # or implement a basic one). For harmonic test, EQ doesn't matter as much as the hysteresis.
    
    # Let's just process the core Hysteresis for Test 1
    # Derivative
    dx = np.diff(x, prepend=0) * fs
    h = 0.5
    
    driven = drive * (x + asym*0.1) - h * langevin(beta_hyst * dx * 0.001)
    y = langevin(driven)
    
    return y

# --- TEST 1: HARMONICS ---
t = np.linspace(0, 0.05, int(fs * 0.05), endpoint=False)
f0 = 1000.0
x_sine = np.sin(2 * np.pi * f0 * t)

plt.figure(figsize=(12, 8))

configs = [
    ("Neutral (Bias=0, Asym=0)", 0.0, 0.0),
    ("Over-Bias (Bias=+1.0, Asym=0)", 1.0, 0.0),
    ("Under-Bias (Bias=-1.0, Asym=0)", -1.0, 0.0),
    ("Asymmetric (Bias=0, Asym=1.0)", 0.0, 1.0)
]

for i, (title, b, a) in enumerate(configs):
    y = process_tape(x_sine, drive=5.0, bias=b, asym=a)
    
    # FFT
    Y = np.fft.fft(y)
    freqs = np.fft.fftfreq(len(y), 1/fs)
    mag = 20 * np.log10(np.abs(Y) / len(y) + 1e-10)
    
    plt.subplot(2, 2, i+1)
    plt.plot(freqs[:len(freqs)//2], mag[:len(freqs)//2])
    plt.xlim(0, 10000)
    plt.ylim(-100, 0)
    plt.title(title)
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude (dB)")
    plt.grid(True)
    
    # Annotate harmonics
    for h in range(1, 6):
        if mag[int(h*f0 * 0.05)] > -80:
            plt.axvline(h*f0, color='r', alpha=0.3, linestyle='--')

plt.tight_layout()
plt.savefig('harmonics_test.png', dpi=150)

# --- TEST 2: FREQUENCY RESPONSE (Biquads) ---
# We will plot the theoretical frequency response of the filters
def plot_filters(ips, bias, ax, title):
    # Gap Loss (Lowpass)
    fc = min(20000, max(20, (ips * 650.0) * (2.0**(-bias/2.0))))
    b_lp, a_lp = butter(2, fc, btype='low', fs=fs)
    
    # Head Bump (Peak)
    fbump = ips * 3.0
    b_pk, a_pk = iirpeak(fbump, 1.5, fs=fs)
    # Scale peak to +2.5dB
    # simple approximation for plotting
    
    # Faraday (Highpass)
    b_hp, a_hp = butter(1, 25.0, btype='high', fs=fs)
    
    w, h_lp = freqz(b_lp, a_lp, worN=2048, fs=fs)
    w, h_hp = freqz(b_hp, a_hp, worN=2048, fs=fs)
    
    # Plot combined
    mag_lp = 20 * np.log10(np.abs(h_lp) + 1e-10)
    mag_hp = 20 * np.log10(np.abs(h_hp) + 1e-10)
    
    # Mocking the peak filter magnitude boost
    w_pk = w
    mag_pk = 2.5 * np.exp(-0.5 * ((w_pk - fbump) / (fbump/2))**2) 
    
    total_mag = mag_lp + mag_hp + mag_pk
    
    ax.plot(w, total_mag, color='b')
    ax.set_xscale('log')
    ax.set_xlim(10, 20000)
    ax.set_ylim(-30, 5)
    ax.set_title(title)
    ax.grid(True, which="both", ls="-", alpha=0.5)
    ax.axhline(0, color='black', linewidth=1)

fig, axs = plt.subplots(2, 2, figsize=(12, 8))
plot_filters(15.0, 0.0, axs[0, 0], "15 IPS (Standard)")
plot_filters(30.0, 0.0, axs[0, 1], "30 IPS (Hi-Fi)")
plot_filters(7.5, 0.0, axs[1, 0], "7.5 IPS (Lo-Fi)")
plot_filters(15.0, 1.0, axs[1, 1], "15 IPS (Over-Biased B=+1.0)")

plt.tight_layout()
plt.savefig('frequency_test.png', dpi=150)
