#!/usr/bin/env python3
"""
N-Body Simulation Orbit Visualizer (Lightweight Version)
Reads output.csv and creates an animated visualization of particle orbits.
No external dependencies except matplotlib.
"""

import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# Read the simulation data
print("Loading simulation data...")
data = {'time': [], 'id': [], 'mass': [], 'x': [], 'y': [], 'z': [], 
        'vx': [], 'vy': [], 'vz': []}

with open('../data/output.csv', 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        data['time'].append(float(row['time']))
        data['id'].append(int(row['id']))
        data['mass'].append(float(row['mass']))
        data['x'].append(float(row['x']))
        data['y'].append(float(row['y']))
        data['z'].append(float(row['z']))
        data['vx'].append(float(row['vx']))
        data['vy'].append(float(row['vy']))
        data['vz'].append(float(row['vz']))

# Convert to numpy arrays for easier manipulation
for key in data:
    data[key] = np.array(data[key])

# Get unique particle IDs
particle_ids = np.unique(data['id'])
num_particles = len(particle_ids)

print(f"Loaded {len(data['time'])} data points for {num_particles} particles")

# Set up the figure and axis
fig, ax = plt.subplots(figsize=(12, 10))
ax.set_aspect('equal')
ax.grid(True, alpha=0.3, linestyle='--')
ax.set_facecolor('#0a0a0a')
fig.patch.set_facecolor('#1a1a1a')

# Color scheme for particles (Sun = yellow/orange, Earth = blue, etc.)
colors = ['#FDB813', '#4A90E2', '#E74C3C', '#2ECC71', '#9B59B6', '#F39C12']

# Store particle data organized by ID
particle_data = {}
for pid in particle_ids:
    mask = data['id'] == pid
    particle_data[pid] = {
        'x': data['x'][mask],
        'y': data['y'][mask],
        'z': data['z'][mask],
        'mass': data['mass'][mask][0],
        'color': colors[int(pid) % len(colors)]
    }

# Calculate plot limits based on data
all_x = data['x']
all_y = data['y']
max_range = max(np.abs(all_x).max(), np.abs(all_y).max()) * 1.15

ax.set_xlim(-max_range, max_range)
ax.set_ylim(-max_range, max_range)
ax.set_xlabel('X Position (m)', color='white', fontsize=13, fontweight='bold')
ax.set_ylabel('Y Position (m)', color='white', fontsize=13, fontweight='bold')
ax.tick_params(colors='white', labelsize=10)

# Format axis labels to use scientific notation
ax.ticklabel_format(style='scientific', axis='both', scilimits=(0,0))

# Title
title = ax.text(0.5, 0.98, '', transform=ax.transAxes, 
                ha='center', va='top', color='white', fontsize=16, fontweight='bold')

# Info text (bottom left)
info_text = ax.text(0.02, 0.02, '', transform=ax.transAxes,
                   ha='left', va='bottom', color='white', fontsize=10,
                   bbox=dict(boxstyle='round', facecolor='#2a2a2a', alpha=0.8))

# Initialize plot elements
particles = []
trails = []
trail_length = 100  # Number of points to show in trail

for pid in particle_ids:
    # Create particle marker (size based on mass)
    mass = particle_data[pid]['mass']
    if mass > 1e29:  # Sun-like
        size = 15
        marker = '*'
    elif mass > 1e25:  # Planet-like
        size = 10
        marker = 'o'
    else:  # Moon-like
        size = 6
        marker = 'o'
    
    particle, = ax.plot([], [], marker, color=particle_data[pid]['color'], 
                       markersize=size, markeredgecolor='white', 
                       markeredgewidth=0.5, label=f'Body {pid}')
    particles.append(particle)
    
    # Create trail
    trail, = ax.plot([], [], '-', color=particle_data[pid]['color'], 
                    alpha=0.5, linewidth=1.5)
    trails.append(trail)

ax.legend(loc='upper right', facecolor='#2a2a2a', edgecolor='white', 
         labelcolor='white', fontsize=11, framealpha=0.9)

# Get total number of frames
num_frames = len(particle_data[particle_ids[0]]['x'])

def init():
    """Initialize animation"""
    for particle in particles:
        particle.set_data([], [])
    for trail in trails:
        trail.set_data([], [])
    title.set_text('')
    info_text.set_text('')
    return particles + trails + [title, info_text]

def animate(frame):
    """Update animation for each frame"""
    # Update title with time
    time_days = frame
    time_years = time_days / 365.25
    title.set_text(f'N-Body Orbital Simulation')
    
    # Calculate total kinetic energy for display
    total_ke = 0
    for pid in particle_ids:
        if frame < len(particle_data[pid]['x']):
            # Note: velocity data is in the CSV but we're just showing position
            pass
    
    info_text.set_text(f'Day: {time_days}\nYear: {time_years:.2f}')
    
    # Update each particle
    for i, pid in enumerate(particle_ids):
        if frame < len(particle_data[pid]['x']):
            x = particle_data[pid]['x'][frame]
            y = particle_data[pid]['y'][frame]
            
            # Update particle position
            particles[i].set_data([x], [y])
            
            # Update trail
            start_idx = max(0, frame - trail_length)
            trail_x = particle_data[pid]['x'][start_idx:frame+1]
            trail_y = particle_data[pid]['y'][start_idx:frame+1]
            trails[i].set_data(trail_x, trail_y)
    
    return particles + trails + [title, info_text]

# Create animation
print("Creating animation...")
print(f"Total frames: {num_frames}")
print(f"Time span: {num_frames} days (~{num_frames/365.25:.2f} years)")

anim = animation.FuncAnimation(fig, animate, init_func=init,
                              frames=num_frames, interval=30, 
                              blit=True, repeat=True)

plt.tight_layout()

try:
    print("Saving animation to orbit_animation.gif... (this may take a moment)")
    anim.save('orbit_animation.gif', writer='pillow', fps=30)
    print("Animation saved as scripts/orbit_animation.gif")
except Exception as e:
    print(f"Could not save GIF: {e}")

print("\nAttempting to show interactive plot... Close the window to exit.")
try:
    plt.show()
except Exception as e:
    print(f"Could not show plot: {e}")

print("\nAnimation closed.")
