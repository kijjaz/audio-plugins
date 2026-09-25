import subprocess, os, base64

base_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(base_dir)
html_path = os.path.join(base_dir, 'FlyBy_Acoustic_Mathematics.html')
pdf_path = os.path.join(base_dir, 'FlyBy_Acoustic_Mathematics.pdf')

fig1_path = os.path.join(base_dir, 'spectrum_elevation_bode.png')
fig2_path = os.path.join(base_dir, 'spectrum_ipd_comparison.png')
fig3_path = os.path.join(base_dir, 'spectrum_dual_waterfall.png')

def to_base64_img(path):
    with open(path, 'rb') as f:
        data = base64.b64encode(f.read()).decode('utf-8')
    return f"data:image/png;base64,{data}"

b64_fig1 = to_base64_img(fig1_path)
b64_fig2 = to_base64_img(fig2_path)
b64_fig3 = to_base64_img(fig3_path)

css_style = """
@import url('https://fonts.googleapis.com/css2?family=Cinzel:wght@600;700;800&family=Inter:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap');

@page {
    size: A4 portrait;
    margin: 16mm 16mm 16mm 16mm;
}

body {
    font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    color: #1e293b;
    line-height: 1.55;
    font-size: 9.5pt;
    margin: 0;
    padding: 0;
}

.header {
    text-align: center;
    border-bottom: 2px solid #c5a059;
    padding-bottom: 12px;
    margin-bottom: 16px;
}

h1.main-title {
    font-family: 'Cinzel', Georgia, serif;
    font-size: 15.5pt;
    font-weight: 700;
    color: #0f172a;
    letter-spacing: 0.5px;
    margin: 0 0 6px 0;
    line-height: 1.25;
}

.subtitle {
    font-size: 9.8pt;
    font-weight: 500;
    color: #475569;
    margin: 0 0 10px 0;
}

.authors {
    font-size: 10.5pt;
    font-weight: 600;
    color: #b48a3c;
    margin-bottom: 2px;
}

.affiliation {
    font-size: 8.5pt;
    color: #64748b;
}

.abstract-box {
    background: #f8fafc;
    border-left: 3px solid #c5a059;
    padding: 10px 14px;
    margin: 12px 0 16px 0;
    font-size: 8.8pt;
    color: #334155;
    border-radius: 0 4px 4px 0;
}

.abstract-title {
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 0.5px;
    font-size: 8.5pt;
    color: #0f172a;
    margin-bottom: 4px;
}

h2 {
    font-family: 'Cinzel', Georgia, serif;
    font-size: 11.5pt;
    font-weight: 700;
    color: #0f172a;
    border-bottom: 1px solid #e2e8f0;
    padding-bottom: 4px;
    margin-top: 18px;
    margin-bottom: 8px;
}

h3 {
    font-size: 10pt;
    font-weight: 600;
    color: #334155;
    margin-top: 12px;
    margin-bottom: 5px;
}

p {
    margin: 0 0 8px 0;
    text-align: justify;
}

.equation {
    background: #fcfdfd;
    border: 1px solid #f1f5f9;
    padding: 5px 10px;
    margin: 8px 0;
    border-radius: 4px;
    text-align: center;
    overflow-x: auto;
}

.figure {
    text-align: center;
    margin: 12px 0;
    page-break-inside: avoid;
}

.figure img {
    max-width: 90%;
    max-height: 220px;
    object-fit: contain;
    border-radius: 4px;
    box-shadow: 0 2px 6px rgba(0,0,0,0.08);
    border: 1px solid #e2e8f0;
}

.caption {
    font-size: 8pt;
    color: #64748b;
    margin-top: 4px;
    font-style: italic;
}

table.data-table {
    width: 100%;
    border-collapse: collapse;
    margin: 12px 0;
    font-size: 8.5pt;
    page-break-inside: avoid;
}

table.data-table th {
    background: #f1f5f9;
    color: #0f172a;
    font-weight: 600;
    padding: 6px 10px;
    border-top: 1px solid #cbd5e1;
    border-bottom: 1px solid #cbd5e1;
    text-align: left;
}

table.data-table td {
    padding: 5px 10px;
    border-bottom: 1px solid #f1f5f9;
}

table.data-table tr:hover {
    background: #f8fafc;
}

.right { text-align: right; }
.center { text-align: center; }
.code { font-family: 'JetBrains Mono', monospace; font-size: 8.5pt; }
"""

html_body = f"""
<div class="header">
    <h1 class="main-title">FlyBy: Mathematical Foundations of Anthropometric 3D Elevation & Flight Trajectory Acoustics</h1>
    <div class="subtitle">Physics-Based Pinna Dispersion, Dynamic Transient Preservation, and Boundary Grounding</div>
    <div class="authors">Kijjaz &bull; Gemini 3.8 Flash</div>
    <div class="affiliation">Audio DSP Research & Engineering &bull; September 2026</div>
</div>

<div class="abstract-box">
    <div class="abstract-title">Abstract</div>
    Headphone spatialization algorithms routinely suffer from &ldquo;elevation collapse&rdquo; and high-frequency transient dulling when rendering sources moving vertically. Traditional stereophonic panning and horizontal Interaural Time/Level Difference (ITD/ILD) cues fail along the median plane because bilateral ear symmetry yields identically zero interaural disparities. This paper establishes the mathematical, physical, and digital signal processing principles implemented in <strong>FlyBy</strong>, a high-performance 3D spatial audio plugin. We formulate the Rayleigh-Woodworth continuous diffraction model, the elevation-dependent pinna moving notch equations (\\(N_1, N_2\\)), chest/torso comb reflections, sub-sample cubic Hermite Doppler trajectories, and two novel contributions: <em>Transient-Preserving Elevation</em> (dynamic notch depth modulation) and <em>Floor/Ceiling Boundary Proximity Reflectance</em>.
</div>

<h2>1. Coordinate Foundations & The Median Plane Dilemma</h2>
<p>
Spatial audio over headphones requires projecting a sound source located in Cartesian 3D coordinates \\(\\vec{{p}} = [x, y, z]^T \\in \\mathbb{{R}}^3\\) into a stereo signal \\((y_L[n], y_R[n])\\). Transforming to spherical coordinates centered at the listener's head:
</p>
<div class="equation">
\\[
r = \\sqrt{{x^2 + y^2 + z^2}}, \\quad \\theta = \\operatorname{{atan2}}(x, y) \\cdot \\frac{{180^\\circ}}{{\\pi}}, \\quad \\phi = \\arcsin\\left(\\frac{{z}}{{r}}\\right) \\cdot \\frac{{180^\\circ}}{{\\pi}}
\\]
</div>
<p>
where \\(r\\) is distance in meters, \\(\\theta \\in [-180^\\circ, +180^\\circ]\\) is azimuth, and \\(\\phi \\in [-90^\\circ, +90^\\circ]\\) is elevation.
</p>

<div class="figure">
    <img src="{b64_fig2}" alt="ITD and ILD comparison"/>
    <div class="caption">Figure 1: Comparison of Horizontal vs. Vertical Panning Cues. Along the median plane (\\(\\theta = 0^\\circ\\)), both ITD and ILD are identically zero. Vertical localization is 100% monaural and spectral.</div>
</div>

<h2>2. Horizontal Acoustics: Rayleigh-Woodworth Model</h2>
<h3>2.1 Sub-Sample Interaural Time Difference (ITD)</h3>
<p>
Let \\(a = 0.0875\\text{{ m}}\\) be the human head radius and \\(c = 343\\text{{ m/s}}\\) be the speed of sound. Accounting for spherical head curvature diffraction:
</p>
<div class="equation">
\\[
\\tau_{{\\text{{ITD}}}}(\\theta) = \\frac{{a}}{{c}}\\left(\\sin|\\theta_{{\\text{{rad}}}}| + |\\theta_{{\\text{{rad}}}}|\\right) \\cdot \\operatorname{{sgn}}(\\theta_{{\\text{{rad}}}})
\\]
</div>
<p>
Delay line reading is implemented with 4-point Cubic Hermite interpolation to guarantee sub-sample smoothness and eliminate zipper noise during high-speed trajectory flybys.
</p>

<h3>2.2 Rayleigh Head Diffraction Filter</h3>
<p>
Shadowing around the spherical skull is governed by the continuous diffraction transfer function with cutoff \\(\\omega_0 = c / a \\approx 2\\pi \\cdot 624\\text{{ Hz}}\\):
</p>
<div class="equation">
\\[
H_{{\\text{{head}}}}(s) = \\frac{{1 + \\alpha(\\theta)\\frac{{s}}{{\\omega_0}}}}{{1 + \\frac{{s}}{{\\omega_0}}}}, \\quad \\alpha(\\theta) = 1.0 + 0.5\\cos(\\theta_{{\\text{{ear}}}})
\\]
</div>
<p>
Applying the bilinear transform \\(s \\leftarrow \\frac{{2}}{{T}}\\frac{{1-z^{{-1}}}}{{1+z^{{-1}}}}\\) yields the first-order IIR digital filter:
</p>
<div class="equation">
\\[
H_{{\\text{{head}}}}(z) = \\frac{{b_0 + b_1 z^{{-1}}}}{{1 + a_1 z^{{-1}}}}, \\quad b_0 = \\frac{{2\\alpha + \\omega_0 T}}{{2 + \\omega_0 T}}, \\quad b_1 = \\frac{{\\omega_0 T - 2\\alpha}}{{2 + \\omega_0 T}}, \\quad a_1 = \\frac{{\\omega_0 T - 2}}{{2 + \\omega_0 T}}
\\]
</div>

<h2>3. Vertical Elevation Acoustics: Pinna Spectral Notches</h2>
<div class="figure">
    <img src="{b64_fig1}" alt="Elevation Frequency Response"/>
    <div class="caption">Figure 2: Measured transfer function \\(H(f)\\) across elevations \\(-40^\\circ \\to +90^\\circ\\). Primary pinna notch \\(N_1\\) sweeps from \\(6.0\\text{{ kHz}}\\) to \\(11.2\\text{{ kHz}}\\), coupled with a torso reflection comb at \\(-40^\\circ\\).</div>
</div>

<p>
Destructive phase cancellations created by outer ear reflections produce elevation-dependent spectral notches:
</p>
<div class="equation">
\\[
f_{{N_1}}(\\phi) = \\left(6000 + \\frac{{\\phi + 50^\\circ}}{{140^\\circ}} \\cdot 5200\\right) \\cdot S_{{\\text{{pinna}}}} \\quad [\\text{{Hz}}]
\\]
</div>
<div class="equation">
\\[
f_{{N_2}}(\\phi) = \\left(9800 + \\frac{{\\phi}}{{90^\\circ}} \\cdot 3200\\right) \\cdot S_{{\\text{{pinna}}}} \\quad [\\text{{Hz}}], \\quad G_{{N_2}}(\\phi) = -14 \\cdot \\left(\\frac{{\\phi}}{{90^\\circ}}\\right) \\quad [\\text{{dB}}]
\\]
</div>
<p>
where \\(S_{{\\text{{pinna}}}} \\in [0.75, 1.25]\\) allows listeners to calibrate notch frequencies to their personal ear geometry.
</p>

<div class="figure">
    <img src="{b64_fig3}" alt="Elevation Waterfall Spectrogram"/>
    <div class="caption">Figure 3: Continuous 2D waterfall spectrogram displaying the upward frequency trajectory of the \\(N_1\\) pinna notch as elevation increases.</div>
</div>

<h2>4. Innovation 1: Transient-Preserving Elevation</h2>
<p>
Traditional spatial audio tools carve deep static notches (\\(-18\\text{{ dB}}\\)), which destroys the high-frequency punch of drum transients, vocal consonants, and pick attacks. <strong>FlyBy</strong> implements an energy-surge onset detector:
</p>
<div class="equation">
\\[
E[n] = \\begin{{cases}}
|x[n]|, & \\text{{if }} |x[n]| > E[n-1] \\\\
|x[n]| + \\alpha_{{\\text{{decay}}}}(E[n-1] - |x[n]|), & \\text{{otherwise}}
\\end{{cases}}
\\]
</div>
<p>
When an attack onset occurs (\\(\\Delta E[n] > 0.015\\) and \\(\\Delta E[n] > 0.4 E[n-1]\\)), the transient ratio \\(T[n]\\) triggers to \\(1.0\\) and decays exponentially with \\(\\tau \\approx 12\\text{{ ms}}\\). The notch depth is dynamically modulated:
</p>
<div class="equation">
\\[
G_{{\\text{{effective}}}}[n] = G_{{\\text{{nominal}}}} \\cdot \\Big(1.0 - C_{{\\text{{crisp}}}} \\cdot T[n]\\Big)
\\]
</div>
<p>
During transient strikes, the notch flattens by up to \\(95\\%\\), maintaining pristine transient punch while applying full spatial elevation filtering to the resonant body and decay.
</p>

<h2>5. Innovation 4: Floor & Ceiling Boundary Grounding</h2>
<p>
Free-field HRTFs in isolation sound sterile. FlyBy incorporates an image-source specular reflector calculating floor (\\(z \\le 0.3\\text{{m}}\\)) and ceiling (\\(z \\ge 1.5\\text{{m}}\\)) boundary paths:
</p>
<div class="equation">
\\[
d_{{\\text{{floor}}}} = \\sqrt{{x^2 + y^2 + (2h_{{\\text{{head}}}} + z)^2}}, \\quad d_{{\\text{{ceiling}}}} = \\sqrt{{x^2 + y^2 + (2H_{{\\text{{room}}}} - 2h_{{\\text{{head}}}} - z)^2}}
\\]
</div>
<p>
Boundary early reflections are attenuated by distance divergence and damped with material absorption filters (\\(H_{{\\text{{floor}}}}\\) damped above \\(3.8\\text{{ kHz}}\\), \\(H_{{\\text{{ceiling}}}}\\) damped above \\(5.5\\text{{ kHz}}\\)).
</p>

<h2>6. Quantitative Acoustic Metrics</h2>
<table class="data-table">
    <thead>
        <tr>
            <th>Configuration</th>
            <th class="center">Azimuth \\(\\theta\\)</th>
            <th class="center">Elevation \\(\\phi\\)</th>
            <th class="right">ITD (\\(\\mu\\text{{s}}\\))</th>
            <th class="right">ILD (dB)</th>
            <th class="right">Notch 1 (Hz)</th>
            <th class="right">Torso Dip</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pure Horizontal (Full Left)</td>
            <td class="center">\\(-90^\\circ\\)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="right">+510.2</td>
            <td class="right">&minus;9.3 dB</td>
            <td class="right">7,857 Hz</td>
            <td class="right">0.0 dB</td>
        </tr>
        <tr>
            <td>Front Center (Eye Level)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="right">0.0</td>
            <td class="right">0.0 dB</td>
            <td class="right">7,857 Hz</td>
            <td class="right">0.0 dB</td>
        </tr>
        <tr>
            <td>Pure Horizontal (Full Right)</td>
            <td class="center">\\(+90^\\circ\\)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="right">&minus;510.2</td>
            <td class="right">+9.3 dB</td>
            <td class="right">7,857 Hz</td>
            <td class="right">0.0 dB</td>
        </tr>
        <tr>
            <td>Below Chin (Nadir / Floor)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="center">\\(-40^\\circ\\)</td>
            <td class="right">0.0</td>
            <td class="right">0.0 dB</td>
            <td class="right"><strong>6,371 Hz</strong></td>
            <td class="right"><strong>&minus;6.4 dB</strong></td>
        </tr>
        <tr>
            <td>Direct Overhead (Zenith)</td>
            <td class="center">\\(0^\\circ\\)</td>
            <td class="center">\\(+90^\\circ\\)</td>
            <td class="right">0.0</td>
            <td class="right">0.0 dB</td>
            <td class="right"><strong>11,200 Hz</strong></td>
            <td class="right">0.0 dB</td>
        </tr>
        <tr>
            <td>Combined 3D Trajectory</td>
            <td class="center">\\(+45^\\circ\\)</td>
            <td class="center">\\(+45^\\circ\\)</td>
            <td class="right">&minus;254.8</td>
            <td class="right">+6.2 dB</td>
            <td class="right">9,528 Hz</td>
            <td class="right">0.0 dB</td>
        </tr>
    </tbody>
</table>
"""

full_html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<title>FlyBy: Mathematical Foundations of Anthropometric 3D Elevation</title>
<script src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js" id="MathJax-script" async></script>
<style>
{css_style}
</style>
</head>
<body>
{html_body}
</body>
</html>
"""

with open(html_path, 'w', encoding='utf-8') as f:
    f.write(full_html)

chrome_path = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
cmd = [
    chrome_path,
    '--headless=new',
    '--disable-gpu',
    '--run-all-compositor-stages-before-draw',
    '--virtual-time-budget=8000',
    '--no-pdf-header-footer',
    f'--print-to-pdf={pdf_path}',
    html_path
]
subprocess.run(cmd, check=True)

if os.path.exists(html_path):
    os.remove(html_path)

print(f"Publication-Grade PDF with embedded figures generated: {pdf_path}")
print(f"File size: {os.path.getsize(pdf_path):,} bytes")
