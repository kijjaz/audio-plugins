import math

WIDTH, HEIGHT = 800, 600

def svg_header(w, h):
    return f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">'

def svg_footer():
    return '</svg>'

def rect(x, y, w, h, fill, r=0, stroke=None, stroke_width=0):
    s = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{r}" fill="{fill}" '
    if stroke:
        s += f'stroke="{stroke}" stroke-width="{stroke_width}" '
    s += '/>'
    return s

def circle(cx, cy, r, fill, stroke=None):
    s = f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="{fill}" '
    if stroke:
        s += f'stroke="{stroke}" stroke-width="2" '
    s += '/>'
    return s

def line(x1, y1, x2, y2, stroke, width=1):
    return f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="{stroke}" stroke-width="{width}" />'

def text(x, y, content, fill, size=12, anchor="middle", weight="normal"):
    return f'<text x="{x}" y="{y}" fill="{fill}" font-family="sans-serif" font-size="{size}" text-anchor="{anchor}" font-weight="{weight}">{content}</text>'

# --- Components ---

def draw_knob(x, y, label):
    s = ""
    # Body
    s += circle(x, y, 15, "#cc9933", "#aa7722") # Gold
    # Indicator
    s += line(x, y, x, y-12, "black", 2)
    # Label
    s += text(x, y+30, label, "#eeeeee", 10)
    return s

def draw_opamp(x, y, model):
    w, h = 40, 50
    s = ""
    # Legs (8)
    for i in range(4):
        ly = y + 5 + i*12
        s += rect(x-5, ly, 5, 8, "#cccccc")
        s += rect(x+w, ly, 5, 8, "#cccccc")

    # Body
    fill = "#222222"
    txt_col = "#aaaaaa"
    lbl = "4558"

    if model == "TL072":
        fill = "#b8860b" # Gold
        txt_col = "white"
        lbl = "TL072"
    elif model == "Broken":
        fill = "#3a2f2f" # Rust
        txt_col = "#665555"
    
    s += rect(x, y, w, h, fill, 2)
    s += text(x+w/2, y+h/2+4, lbl, txt_col, 10, "middle", "bold")

    # Crack
    if model == "Broken":
        s += f'<path d="M{x},{y} L{x+w},{y+h} M{x},{y+h} L{x+w},{y}" stroke="black" stroke-width="1.5" />'

    return s

def draw_diode(x, y, model):
    s = ""
    if model == "LED":
        # Glow
        s += circle(x, y, 8, "rgba(255,0,0,0.3)")
        s += circle(x, y, 4, "red")
    else:
        # Leads
        s += line(x-10, y, x+10, y, "#cccccc", 1)
        # Body
        fill = "#ff4400" if model=="Silicon" else "#e0f7fa" # Orange / Clear
        s += rect(x-6, y-3, 12, 6, fill)
        # Band
        s += rect(x+2, y-3, 4, 6, "black")
    return s

def draw_cap(x, y, aged):
    s = ""
    w, h = 15, 25
    fill = "#222244" if not aged else "#555566"
    
    # Body
    s += rect(x, y, w, h, fill)
    
    # Cap (Silver)
    # SVG Ellipse
    cy_top = 8 # height of ellipse
    if aged: cy_top = 12 # Bulge
    
    s += f'<ellipse cx="{x+w/2}" cy="{y}" rx="{w/2}" ry="{cy_top/2}" fill="#cccccc" />'

    # Leak
    if aged:
        s += circle(x+w+2, y+h, 3, "#663300")
    
    return s

# --- Master Draw ---
svg = svg_header(WIDTH, HEIGHT)

# BG
svg += rect(0, 0, WIDTH, HEIGHT, "#222222")

# Header
svg += text(WIDTH/2, 50, "DISCRETE 808", "#b8860b", 36, "middle", "bold")

# Channel Strips
channels = ["KICK", "SNARE", "LT", "MT", "HT", "CLAP", "CH", "OH"]
start_x = 50
spacing = 90
y_knobs = 150

for i, name in enumerate(channels):
    cx = start_x + i * spacing
    svg += text(cx, y_knobs - 40, name, "#cc9933", 14, "middle", "bold")
    svg += draw_knob(cx, y_knobs, "Level")
    svg += draw_knob(cx, y_knobs + 70, "Tune")
    svg += draw_knob(cx, y_knobs + 140, "Decay")

# Hatch
hatch_y = 450
svg += rect(100, hatch_y, 600, 120, "#111111", 10, "#444444", 2)
svg += text(120, hatch_y - 15, "MAINTENANCE HATCH / CIRCUIT MODS", "#999999", 12, "start")

# OpAmps
svg += draw_opamp(150, hatch_y + 35, "Standard")
svg += draw_opamp(220, hatch_y + 35, "TL072")
svg += draw_opamp(290, hatch_y + 35, "Broken")

# Diodes
svg += draw_diode(400, hatch_y + 60, "Silicon")
svg += draw_diode(430, hatch_y + 60, "Germanium")
svg += draw_diode(460, hatch_y + 60, "LED")

# Caps
svg += draw_cap(550, hatch_y + 50, False)
svg += draw_cap(600, hatch_y + 50, True)
svg += text(640, hatch_y + 70, "AGED", "red", 10, "start")


svg += svg_footer()

with open("discrete_808_preview.svg", "w") as f:
    f.write(svg)

print("SVG Generated.")
