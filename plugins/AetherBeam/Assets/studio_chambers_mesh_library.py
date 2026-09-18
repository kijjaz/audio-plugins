"""
Acoustic Chamber Mesh Generator:
1. Capitol Studios Underground Echo Chamber 4 (Los Angeles, CA - Les Paul Design)
   - Trapezoidal non-parallel concrete walls, sloping ceiling, high-gloss organ lacquer.
2. Abbey Road Studios Echo Chamber 2 (London, UK - EMI Design)
   - Tiled walls, glossy paint, and vertical glazed ceramic drainage pipe diffusers.
"""

import numpy as np
from mesh_acoustic_simulation import AcousticMesh

def build_capitol_chamber_mesh():
    """
    Capitol Studios Underground Echo Chamber (Chamber 4).
    Volume: ~57 m3. RT60: ~4.2s.
    Features: Asymmetric trapezoid, sloping ceiling (no parallel surfaces),
    10-inch thick concrete sealed with high-gloss organ lacquer.
    Dimensions:
      Length = 6.8 m
      Front Width (narrow) = 3.1 m (at Y=0)
      Rear Width (wide) = 4.2 m (at Y=6.8)
      Front Ceiling Height = 2.8 m
      Rear Ceiling Height = 3.4 m
    """
    mesh = AcousticMesh()
    mesh.materials.update({
        'lacquered_concrete': 0.012, # Ultra-reflective hard gloss lacquer
        'floor_concrete': 0.015
    })
    
    length = 6.8
    w_front = 3.1
    w_rear = 4.2
    h_front = 2.8
    h_rear = 3.4
    
    # 4 Floor Vertices (Z = 0)
    # Front narrow wall at Y = 0
    f0 = mesh.add_vertex(-w_front/2, 0.0, 0.0)      # Front-Left
    f1 = mesh.add_vertex(w_front/2, 0.0, 0.0)       # Front-Right
    # Rear wide wall at Y = length
    f2 = mesh.add_vertex(w_rear/2, length, 0.0)     # Rear-Right
    f3 = mesh.add_vertex(-w_rear/2, length, 0.0)    # Rear-Left
    mesh.add_quad(f0, f1, f2, f3, 'floor_concrete')
    
    # 4 Ceiling Vertices (Sloping Z)
    c0 = mesh.add_vertex(-w_front/2, 0.0, h_front)  # Ceiling Front-Left
    c1 = mesh.add_vertex(w_front/2, 0.0, h_front)   # Ceiling Front-Right
    c2 = mesh.add_vertex(w_rear/2, length, h_rear)  # Ceiling Rear-Right
    c3 = mesh.add_vertex(-w_rear/2, length, h_rear) # Ceiling Rear-Left
    # Ceiling quad (facing down into chamber)
    mesh.add_quad(c0, c3, c2, c1, 'lacquered_concrete')
    
    # 4 Angled Perimeter Walls
    # Front wall (Y=0)
    mesh.add_quad(f0, c0, c1, f1, 'lacquered_concrete')
    # Right wall (angled outward)
    mesh.add_quad(f1, c1, c2, f2, 'lacquered_concrete')
    # Rear wall (Y=length)
    mesh.add_quad(f2, c2, c3, f3, 'lacquered_concrete')
    # Left wall (angled outward)
    mesh.add_quad(f3, c3, c0, f0, 'lacquered_concrete')
    
    return mesh

def build_abbey_road_chamber_mesh():
    """
    Abbey Road Studios Echo Chamber 2.
    Volume: ~66 m3. RT60: ~2.8s.
    Features: 6.4m x 3.7m x 2.8m room with hard glazed wall tiles,
    plus 4 freestanding vertical cylindrical ceramic drainage pipe diffusers
    positioned on the floor to break up flutter echoes.
    """
    mesh = AcousticMesh()
    mesh.materials.update({
        'glazed_tile': 0.016,     # Ceramic glazed wall tiles
        'tiled_floor': 0.015,     # Hard tiled floor
        'gloss_ceiling': 0.020,   # Heavy gloss enamel painted ceiling
        'ceramic_pipe': 0.012     # Glazed clay drainage pipe diffusers
    })
    
    length = 6.4
    width = 3.7
    height = 2.8
    
    # 1. Main Room Enclosure
    # Floor (Z=0)
    f0 = mesh.add_vertex(-width/2, 0.0, 0.0)
    f1 = mesh.add_vertex(width/2, 0.0, 0.0)
    f2 = mesh.add_vertex(width/2, length, 0.0)
    f3 = mesh.add_vertex(-width/2, length, 0.0)
    mesh.add_quad(f0, f1, f2, f3, 'tiled_floor')
    
    # Ceiling (Z=height)
    c0 = mesh.add_vertex(-width/2, 0.0, height)
    c1 = mesh.add_vertex(width/2, 0.0, height)
    c2 = mesh.add_vertex(width/2, length, height)
    c3 = mesh.add_vertex(-width/2, length, height)
    mesh.add_quad(c0, c3, c2, c1, 'gloss_ceiling')
    
    # Walls
    mesh.add_quad(f0, c0, c1, f1, 'glazed_tile') # Front wall
    mesh.add_quad(f1, c1, c2, f2, 'glazed_tile') # Right wall
    mesh.add_quad(f2, c2, c3, f3, 'glazed_tile') # Back wall
    mesh.add_quad(f3, c3, c0, f0, 'glazed_tile') # Left wall
    
    # 2. Authentic Glazed Ceramic Drainage Pipe Diffusers
    # 4 vertical columns standing upright on the floor (radius ~ 0.28m, height = 2.2m)
    pipe_positions = [
        (-0.75, 2.0),
        (0.70, 2.8),
        (-0.60, 4.2),
        (0.65, 4.8)
    ]
    pipe_radius = 0.26
    pipe_height = 2.2
    n_seg = 12
    
    for px, py in pipe_positions:
        base_ring = []
        top_ring = []
        for i in range(n_seg):
            th = 2 * np.pi * i / n_seg
            x = px + pipe_radius * np.cos(th)
            y = py + pipe_radius * np.sin(th)
            base_ring.append(mesh.add_vertex(x, y, 0.0))
            top_ring.append(mesh.add_vertex(x, y, pipe_height))
            
        # Cylinder side quads
        for i in range(n_seg):
            i_next = (i + 1) % n_seg
            # Normal facing outward from cylinder into the room
            mesh.add_quad(base_ring[i], top_ring[i], top_ring[i_next], base_ring[i_next], 'ceramic_pipe')
            
        # Top cap
        cap_center = mesh.add_vertex(px, py, pipe_height)
        for i in range(n_seg):
            i_next = (i + 1) % n_seg
            mesh.add_triangle(top_ring[i], cap_center, top_ring[i_next], 'ceramic_pipe')
            
    return mesh
