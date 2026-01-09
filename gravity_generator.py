import numpy as np
import pandas as pd

def generate_gravity_data(steps=5000, dt=0.01):
    # Initial conditions (Positions and Velocities)
    # Mass 1 (Sun), Mass 2 (Planet), Mass 3 (Moon)
    m = [100.0, 5.0, 0.1]
    pos = np.array([[0.0,0.0], [10.0,0.0], [11.0,0.0]], dtype=float)
    vel = np.array([[0.0,0.0], [0.0,2.5], [0.0,3.8]], dtype=float) 
    # Adjusted velocities for stable-ish orbit to ensure longer interesting interaction
    
    data = []
    
    # Store max/min for normalization later
    all_pos_history = []

    for _ in range(steps):
        accel = np.zeros_like(pos)
        for i in range(3):
            for j in range(3):
                if i != j:
                    r = pos[j] - pos[i]
                    dist = np.linalg.norm(r) + 0.5 # Softening factor increased to prevent explosion
                    accel[i] += m[j] * r / dist**3
        
        vel += accel * dt
        pos += vel * dt
        
        # Capture current state
        row = []
        for i in range(3):
             row.extend(pos[i])
        
        # Calculate derived metrics for synthesis Mapping
        # 1. Distances (Proximity)
        d12 = np.linalg.norm(pos[0] - pos[1]) # Sun-Planet
        d23 = np.linalg.norm(pos[1] - pos[2]) # Planet-Moon
        d13 = np.linalg.norm(pos[0] - pos[2]) # Sun-Moon
        
        # 2. Velocity Magnitudes (Doppler)
        v1 = np.linalg.norm(vel[0])
        v2 = np.linalg.norm(vel[1])
        v3 = np.linalg.norm(vel[2])

        row.extend([d12, d23, d13, v1, v2, v3])
        data.append(row)

    cols = ['x1','y1','x2','y2','x3','y3', 'd12', 'd23', 'd13', 'v1', 'v2', 'v3']
    df = pd.DataFrame(data, columns=cols)
    
    # --- Normalization for V2 ---
    # We want X/Y to be -1.0 to 1.0 for Pan/Elevation
    # We want Distances/Velocities to be 0.0 to 1.0 (Exponentially friendly)
    
    # 1. Normalize Pos to Centered -1 to 1
    for c in ['x1','y1','x2','y2','x3','y3']:
        col_min = df[c].min()
        col_max = df[c].max()
        # Scale to -1..1
        df[c] = 2.0 * (df[c] - col_min) / (col_max - col_min) - 1.0
        
    # 2. Normalize Dist/Vel to 0..1
    for c in ['d12','d23','d13','v1','v2','v3']:
        col_min = df[c].min()
        col_max = df[c].max()
        df[c] = (df[c] - col_min) / (col_max - col_min)
        
    df = df.round(6)
    
    df.to_csv("gravity_table.csv", index=False, float_format='%.6f')
    print("Generated gravity_table.csv with shape:", df.shape)

if __name__ == "__main__":
    # Increased steps for longer evolution
    generate_gravity_data(steps=10000)
