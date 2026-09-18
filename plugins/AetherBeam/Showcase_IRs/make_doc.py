import subprocess, os

html_path = '/Users/kijjaz/Desktop/Antigravity/2026/20260909 Audio Plugin Development/AetherBeam/Showcase_IRs/doc.html'
pdf_path = '/Users/kijjaz/Desktop/Antigravity/2026/20260909 Audio Plugin Development/AetherBeam/Showcase_IRs/AetherBeam_Mathematical_Acoustic_Engine.pdf'

html_code = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<title>AetherBeam: High-Fidelity Nonlinear Waveguide Acoustic Ray-Tracing Engine</title>
<script src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js" id="MathJax-script" async></script>
<style>
@import url('https://fonts.googleapis.com/css2?family=Cinzel:wght@600;700;800&family=Inter:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap');

@page {
    size: A4 portrait;
    margin: 20mm 18mm 20mm 18mm;
}

body {
    font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    color: #1e293b;
    line-height: 1.65;
    font-size: 10pt;
    margin: 0;
    padding: 0;
}

.header {
    text-align: center;
    border-bottom: 2px solid #c5a059;
    padding-bottom: 16px;
    margin-bottom: 20px;
}

h1.main-title {
    font-family: 'Cinzel', Georgia, serif;
    font-size: 17pt;
    font-weight: 700;
    color: #0f172a;
    letter-spacing: 0.5px;
    margin: 0 0 8px 0;
    line-height: 1.3;
}

.subtitle {
    font-size: 10.5pt;
    font-weight: 500;
    color: #475569;
    margin: 0 0 12px 0;
}

.authors {
    font-size: 11pt;
    font-weight: 600;
    color: #b48a3c;
    margin-bottom: 4px;
}

.affiliation {
    font-size: 9.5pt;
    color: #64748b;
    margin-bottom: 4px;
}

.date {
    font-size: 9pt;
    color: #94a3b8;
}

.abstract-box {
    background: #f8fafc;
    border-left: 4px solid #c5a059;
    padding: 14px 18px;
    margin: 20px 0 24px 0;
    font-size: 9.5pt;
    border-radius: 0 6px 6px 0;
    line-height: 1.6;
}

.abstract-label {
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 1px;
    font-size: 8pt;
    color: #64748b;
    margin-bottom: 6px;
}

h2 {
    font-size: 12.5pt;
    font-weight: 700;
    color: #0f172a;
    border-bottom: 1px solid #e2e8f0;
    padding-bottom: 4px;
    margin-top: 26px;
    margin-bottom: 10px;
    page-break-after: avoid;
}

h3 {
    font-size: 10.5pt;
    font-weight: 600;
    color: #334155;
    margin-top: 16px;
    margin-bottom: 6px;
    page-break-after: avoid;
}

p {
    margin: 8px 0;
    text-align: justify;
}

ul {
    margin: 6px 0 12px 20px;
    padding: 0;
}

li {
    margin-bottom: 4px;
}

.table-container {
    margin: 24px 0;
    page-break-inside: avoid;
}

.table-caption {
    font-weight: 600;
    font-size: 9.5pt;
    color: #334155;
    margin-bottom: 8px;
}

table {
    width: 100%;
    border-collapse: collapse;
    font-size: 8.5pt;
    table-layout: fixed;
}

th {
    background-color: #f8fafc;
    border-top: 2px solid #0f172a;
    border-bottom: 1.5px solid #0f172a;
    font-weight: 600;
    color: #0f172a;
    padding: 9px 8px;
    text-align: left;
    vertical-align: middle;
}

td {
    padding: 9px 8px;
    border-bottom: 1px solid #e2e8f0;
    vertical-align: middle;
}

tr:last-child td {
    border-bottom: 2px solid #0f172a;
}

th.center, td.center { text-align: center; }
th.right, td.right { text-align: right; }

.col-name { width: 25%; }
.col-dim  { width: 22%; }
.col-vol  { width: 14%; }
.col-rt   { width: 11%; }
.col-ord  { width: 8%; }
.col-rays { width: 9%; }
.col-spl  { width: 11%; }

.MathJax {
    font-size: 1.0em !important;
}
</style>
</head>
<body>

<div class="header">
    <h1 class="main-title">AetherBeam: High-Fidelity Nonlinear Waveguide Acoustic Ray-Tracing Engine</h1>
    <div class="subtitle">Mathematical Formulation and Physical Principles of Orders 0–4 Sound Beam Synthesis</div>
    <div class="authors">Kijjaz &amp; Gemini 3.8 Flash</div>
    <div class="affiliation">AetherBeam Reverb DSP Architecture &bull; Advanced Spatial Audio</div>
    <div class="date">September 2026</div>
</div>

<div class="abstract-box">
    <div class="abstract-label">Abstract</div>
    This paper presents the complete mathematical architecture of the <strong>AetherBeam</strong> physical acoustic engine. The system synthesizes spatial impulse responses by combining higher-order 3D geometric image-source ray tracing (Orders \(0 \le n \le 4\), producing up to 96 physical acoustic reflection paths per enclosure), analytic boundary collision validation, and finite-amplitude weakly nonlinear acoustic wave propagation (Fubini-Bessel shock wave steepening under extreme sound pressure levels up to \(122\,\text{dB SPL}\), representative of a full symphony orchestra playing at <em>fff</em>).
</div>

<h2>1. 3D Architectural Image-Source Geometry (Orders 0–4)</h2>
<p>Let the bounding coordinates of an acoustic enclosure \(\Omega \subset \mathbb{R}^3\) be denoted by:</p>
\[
\Omega = [x_{\min}, x_{\max}] \times [y_{\min}, y_{\max}] \times [z_{\min}, z_{\max}]
\]
<p>with enclosure dimensions \(L_x = x_{\max} - x_{\min}\), \(L_y = y_{\max} - y_{\min}\), and \(L_z = z_{\max} - z_{\min}\). Let \(\mathbf{s} = (s_x, s_y, s_z)^\top \in \Omega\) denote the physical sound source, and \(\mathbf{r} = (r_x, r_y, r_z)^\top \in \Omega\) denote the listener position.</p>

<h3>1.1 Lattice Coordinate Transformation</h3>
<p>Relative to the enclosure minimum origin \(\mathbf{b}_{\min}\), coordinates are mapped to \(\mathbf{s}_{\text{rel}} = \mathbf{s} - \mathbf{b}_{\min}\) and \(\mathbf{r}_{\text{rel}} = \mathbf{r} - \mathbf{b}_{\min}\). For reflection index vector \(\mathbf{n} = (n_x, n_y, n_z) \in \mathbb{Z}^3\), the virtual image-source coordinate along axis \(i \in \{x, y, z\}\) is defined by:</p>
\[
s_i'(n_i) = \begin{cases}
n_i L_i + s_{\text{rel}, i}, & \text{if } n_i \equiv 0 \pmod 2 \\
(n_i + 1) L_i - s_{\text{rel}, i}, & \text{if } n_i \equiv 1 \pmod 2
\end{cases}
\]
<p>The total reflection order \(N(\mathbf{n})\) is the \(\ell_1\) norm of the lattice vector:</p>
\[
N(\mathbf{n}) = |n_x| + |n_y| + |n_z|, \quad 0 \le N(\mathbf{n}) \le 4
\]

<h3>1.2 Ray Trajectory and Propagation Metric</h3>
<p>The vector from the virtual image-source \(\mathbf{s}'(\mathbf{n})\) to the receiver \(\mathbf{r}_{\text{rel}}\) is \(\mathbf{d}(\mathbf{n}) = \mathbf{r}_{\text{rel}} - \mathbf{s}'(\mathbf{n})\), with total propagation distance:</p>
\[
d(\mathbf{n}) = \|\mathbf{d}(\mathbf{n})\|_2 = \sqrt{(r_{\text{rel}, x} - s_x')^2 + (r_{\text{rel}, y} - s_y')^2 + (r_{\text{rel}, z} - s_z')^2}
\]
<p>The corresponding acoustic propagation delay in dry air at \(20^\circ\text{C}\) (\(c_0 = 343.2\,\text{m/s}\)) is:</p>
\[
\tau(\mathbf{n}) = \frac{d(\mathbf{n})}{c_0}
\]

<h3>1.3 Trajectory Folding</h3>
<p>The folded spatial position \(\mathbf{p}(t)\) along normalized path parameter \(t \in [0, 1]\) is obtained via periodic coordinate folding:</p>
\[
p_i(t) = \text{fold}\big(s_i' + t \cdot d_i(\mathbf{n}),\, L_i\big) + b_{\min, i}
\]

<h2>2. Strict Architectural Boundary Containment</h2>
<p>To ensure sound rays never escape non-rectangular or vaulted architectures, every reflection point \(\mathbf{p} \in \mathbb{R}^3\) must satisfy the space-specific manifold constraint \(\mathcal{C}_{\text{space}}(\mathbf{p}) \le 0\).</p>

<h3>2.1 Sydney Opera House (Pitched Vault)</h3>
\[
\mathcal{C}_{\text{Sydney}}(\mathbf{p}) = p_z - (25.0 - |p_x|) \le 0
\]
<p>Any virtual ray whose folded reflection apex exceeds \(Z = 25 - |x|\) is immediately pruned.</p>

<h3>2.2 Rome Pantheon (Cylindrical Drum + Hemispherical Dome)</h3>
\[
\mathcal{C}_{\text{Pantheon}}(\mathbf{p}) = \begin{cases}
\sqrt{p_x^2 + p_y^2} - 21.65 \le 0, & \text{if } p_z \le 21.65\,\text{m} \\
p_x^2 + p_y^2 + (p_z - 21.65)^2 - (21.65)^2 \le 0, & \text{if } p_z > 21.65\,\text{m}
\end{cases}
\]

<h3>2.3 Hagia Sophia (Central Dome + Exedrae)</h3>
<p>For the central nave within radius \(r = \sqrt{p_x^2 + p_y^2} \le 15.5\,\text{m}\):</p>
\[
p_z \le 25.0 + 30.0 \sqrt{1 - \left(\frac{r}{15.5}\right)^2}
\]

<h2>3. Finite-Amplitude Nonlinear Wave Steepening (<em>fff</em> Dynamics)</h2>
<p>When a full symphony orchestra plays fortississimo (<em>fff</em>), sound pressure levels near the proscenium reach \(L_p \approx 122\,\text{dB SPL}\), corresponding to peak acoustic pressure:</p>
\[
p_{\text{peak}} = p_{\text{ref}} \cdot 10^{\frac{L_p}{20}} = (20\,\mu\text{Pa}) \cdot 10^{\frac{122}{20}} \approx 25.18\,\text{Pa}
\]

<h3>3.1 Burgers' Equation and Nonlinearity Parameter</h3>
<p>One-dimensional finite-amplitude plane wave propagation is governed by Burgers' equation:</p>
\[
\frac{\partial p}{\partial x} - \frac{\beta p}{\rho_0 c_0^3} \frac{\partial p}{\partial t'} = \frac{\delta}{2 c_0^3} \frac{\partial^2 p}{\partial {t'}^2}
\]
<p>where \(t' = t - x/c_0\) is retarded time, \(\rho_0 = 1.204\,\text{kg/m}^3\), \(c_0 = 343.2\,\text{m/s}\), and \(\beta = 1 + \frac{B}{2A} \approx 1.201\) is the nonlinearity parameter for air.</p>

<h3>3.2 Shock Formation Distance and Fubini-Bessel Series</h3>
<p>The Goldberg distortion parameter \(\sigma(d)\) along path length \(d\) is:</p>
\[
\sigma(d) = \frac{\beta \omega p_{\text{peak}} d}{\rho_0 c_0^3}
\]
<p>For symphonic orchestral dynamics (\(\sigma \in [0.05, 0.45]\)), the acoustic pressure obeys the <strong>Fubini-Bessel series</strong>:</p>
\[
p(d, t') = p_{\text{peak}} \sum_{n=1}^{\infty} \frac{2}{n \sigma(d)} J_n(n \sigma(d)) \sin(n \omega t')
\]

<h3>3.3 Discrete Nonlinear Waveguide Implementation</h3>
<p>At \(f_s = 96\,\text{kHz}\), cumulative wave steepening along path \(k\) of distance \(d_k\) is evaluated via polynomial wave-shaping with dynamic coefficient \(\gamma_k\):</p>
\[
\gamma_k = \frac{\beta \cdot p_{\text{peak}} \cdot d_k}{2 \rho_0 c_0^3}, \qquad y_k[n] = x[n] + \gamma_k \cdot x^2[n] - \frac{1}{3} \gamma_k^2 \cdot x^3[n]
\]

<h2>4. Total Impulse Response Synthesis</h2>
<p>The complete binaural impulse response \(h_{L,R}(t)\) is synthesized as the superposition of deterministic early physical beams (Orders \(0 \le n \le 4\)) and the stochastic diffuse late reverberant tail:</p>
\[
h_{L,R}(t) = \sum_{k=1}^{K} A_k \cdot \mathcal{N}_k\Big( \delta(t - \tau_k) * h_{\text{air}}(t, d_k) \Big) \cdot \text{HRTF}_{L,R}(\theta_k, \phi_k) + h_{\text{late}}(t)
\]
<p>where \(K \le 96\) is the total count of valid physical rays, \(A_k = R_k / d_k\) accounts for spherical attenuation and boundary absorption, \(\mathcal{N}_k(\cdot)\) is the nonlinear wave-steepening operator, and \(h_{\text{late}}(t) = e^{-3 \ln(10) t / RT_{60}} \cdot \xi(t)\) provides the late reverberation tail.</p>

<div class="table-container">
    <div class="table-caption">Table 1: Physical acoustic parameters of synthesized showcase spaces.</div>
    <table>
        <thead>
            <tr>
                <th class="col-name">Enclosure</th>
                <th class="col-dim">Dimensions (m)</th>
                <th class="col-vol right">Volume (m&sup3;)</th>
                <th class="col-rt right">RT<sub>60</sub> (s)</th>
                <th class="col-ord center">Order</th>
                <th class="col-rays center">Valid Rays</th>
                <th class="col-spl right">L<sub>p</sub> (SPL)</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Musikverein Golden Hall</td>
                <td>48.8 &times; 19.1 &times; 17.7</td>
                <td class="right">15,000</td>
                <td class="right">2.05</td>
                <td class="center">4</td>
                <td class="center">76</td>
                <td class="right">122 dB (<em>fff</em>)</td>
            </tr>
            <tr>
                <td>Sydney Opera House</td>
                <td>52.0 &times; 26.0 &times; 25.0</td>
                <td class="right">16,500</td>
                <td class="right">2.00</td>
                <td class="center">4</td>
                <td class="center">68</td>
                <td class="right">122 dB (<em>fff</em>)</td>
            </tr>
            <tr>
                <td>Hagia Sophia Byzantine</td>
                <td>65.0 &times; 60.0 &times; 55.0</td>
                <td class="right">180,000</td>
                <td class="right">11.00</td>
                <td class="center">4</td>
                <td class="center">84</td>
                <td class="right">122 dB (<em>fff</em>)</td>
            </tr>
        </tbody>
    </table>
</div>

</body>
</html>
"""

with open(html_path, 'w', encoding='utf-8') as f:
    f.write(html_code)

chrome_path = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
cmd_chrome = [
    chrome_path,
    '--headless=new',
    '--disable-gpu',
    '--run-all-compositor-stages-before-draw',
    '--virtual-time-budget=6000',
    '--no-pdf-header-footer',
    f'--print-to-pdf={pdf_path}',
    html_path
]
subprocess.run(cmd_chrome, check=True)

if os.path.exists(html_path):
    os.remove(html_path)

print('Clean PDF recompiled! Size:', os.path.getsize(pdf_path))
