import numpy as np

def blackman_harris_lowpass(num_taps, cutoff_hz, fs):
    if num_taps % 2 == 0:
        num_taps += 1
    fc = cutoff_hz / fs
    M = num_taps - 1
    mid = M // 2
    n = np.arange(num_taps)
    
    # Sinc
    sinc = np.zeros(num_taps)
    mask = (n == mid)
    sinc[mask] = 2.0 * fc
    non_mid = ~mask
    sinc[non_mid] = np.sin(2.0 * np.pi * fc * (n[non_mid] - mid)) / (np.pi * (n[non_mid] - mid))
    
    # 4-term Blackman-Harris window
    a0, a1, a2, a3 = 0.35875, 0.48829, 0.14128, 0.01168
    w = a0 - a1 * np.cos(2.0 * np.pi * n / M) + a2 * np.cos(4.0 * np.pi * n / M) - a3 * np.cos(6.0 * np.pi * n / M)
    h = sinc * w
    h /= np.sum(h)
    return h

def test_linear_phase_reconstruction():
    fs = 48000.0
    num_taps = 255
    tau = (num_taps - 1) // 2
    total_tau = tau * 2
    fc1, fc2 = 250.0, 3000.0

    lp1 = blackman_harris_lowpass(num_taps, fc1, fs)
    lp2 = blackman_harris_lowpass(num_taps, fc2, fs)

    # Impulse input
    sig_len = 2048
    x = np.zeros(sig_len)
    x[0] = 1.0

    # Stage 1: split at fc2
    lowmid = np.convolve(x, lp2)[:sig_len]
    delayed_x_tau2 = np.zeros(sig_len)
    delayed_x_tau2[tau:] = x[:-tau]
    high_pre_tau1 = delayed_x_tau2 - lowmid
    high = np.zeros(sig_len)
    high[tau:] = high_pre_tau1[:-tau]

    # Stage 2: split lowmid at fc1
    low = np.convolve(lowmid, lp1)[:sig_len]
    delayed_lowmid_tau1 = np.zeros(sig_len)
    delayed_lowmid_tau1[tau:] = lowmid[:-tau]
    mid = delayed_lowmid_tau1 - low

    # Sum of all three bands
    y_sum = low + mid + high

    # Expected: pure impulse delayed by total_tau = 254 samples
    expected = np.zeros(sig_len)
    expected[total_tau] = 1.0

    error = np.max(np.abs(y_sum - expected))
    error_db = 20.0 * np.log10(error + 1e-15)

    print(f"Reconstruction Error Peak: {error:.2e} ({error_db:.2f} dB)")
    assert error < 1e-5, f"Linear-phase reconstruction error too high: {error}"
    print("SUCCESS: Exact mathematical linear-phase reconstruction confirmed!")

if __name__ == "__main__":
    test_linear_phase_reconstruction()
