#!/usr/bin/env python3
"""
N-Body Simulation Visualizer
Reads data/output.csv and (optionally) data/energy_log.csv.
Produces:
  - An animated GIF of the orbits
  - A static energy conservation plot (if energy_log.csv exists)
"""

import csv
import os
import sys
import matplotlib
matplotlib.use('Agg')          # non-interactive backend — works without a display
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# ── Paths ─────────────────────────────────────────────────────────────────────
SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
DATA_DIR    = os.path.join(SCRIPT_DIR, '..', 'data')
ORBIT_CSV   = os.path.join(DATA_DIR, 'output.csv')
ENERGY_CSV  = os.path.join(DATA_DIR, 'energy_log.csv')
OUT_GIF     = os.path.join(SCRIPT_DIR, 'orbit_animation.gif')
OUT_ENERGY  = os.path.join(SCRIPT_DIR, 'energy_conservation.png')

# ── Body names keyed by particle ID ──────────────────────────────────────────
BODY_NAMES = {0: 'Sun', 1: 'Earth', 2: 'Moon'}

# ── Colour scheme ─────────────────────────────────────────────────────────────
COLORS = ['#FDB813', '#4A90E2', '#C8C8C8', '#E74C3C', '#2ECC71']


# ═════════════════════════════════════════════════════════════════════════════
# 1. Load orbit data
# ═════════════════════════════════════════════════════════════════════════════
def load_orbits(path):
    print(f"Loading orbit data from {path} ...")
    data = {k: [] for k in ('time', 'id', 'mass', 'x', 'y', 'z', 'vx', 'vy', 'vz')}
    with open(path, 'r') as f:
        for row in csv.DictReader(f):
            data['time'].append(float(row['time']))
            data['id'].append(int(row['id']))
            data['mass'].append(float(row['mass']))
            data['x'].append(float(row['x']))
            data['y'].append(float(row['y']))
            data['z'].append(float(row['z']))
            data['vx'].append(float(row['vx']))
            data['vy'].append(float(row['vy']))
            data['vz'].append(float(row['vz']))
    for k in data:
        data[k] = np.array(data[k])

    particle_ids = np.unique(data['id'])
    bodies = {}
    for pid in particle_ids:
        mask = data['id'] == pid
        bodies[pid] = {
            'x':    data['x'][mask],
            'y':    data['y'][mask],
            'z':    data['z'][mask],
            'mass': data['mass'][mask][0],
            'name': BODY_NAMES.get(int(pid), f'Body {pid}'),
            'color': COLORS[int(pid) % len(COLORS)],
        }

    n_frames = len(bodies[particle_ids[0]]['x'])
    print(f"  {len(data['time'])} rows | {len(particle_ids)} bodies | {n_frames} frames")
    return bodies, particle_ids, n_frames


# ═════════════════════════════════════════════════════════════════════════════
# 2. Build orbit animation
# ═════════════════════════════════════════════════════════════════════════════
def make_orbit_animation(bodies, particle_ids, n_frames, out_path):
    fig, ax = plt.subplots(figsize=(12, 10))
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.25, linestyle='--', color='gray')
    ax.set_facecolor('#0a0a0a')
    fig.patch.set_facecolor('#111111')

    # Compute plot bounds from the outermost body (ignore tiny Moon displacement)
    sun_mask  = bodies[particle_ids[0]]['x']
    all_x = np.concatenate([bodies[pid]['x'] for pid in particle_ids])
    all_y = np.concatenate([bodies[pid]['y'] for pid in particle_ids])
    max_range = max(np.abs(all_x).max(), np.abs(all_y).max()) * 1.12
    ax.set_xlim(-max_range, max_range)
    ax.set_ylim(-max_range, max_range)

    ax.set_xlabel('X Position (m)', color='white', fontsize=12, fontweight='bold')
    ax.set_ylabel('Y Position (m)', color='white', fontsize=12, fontweight='bold')
    ax.tick_params(colors='white', labelsize=9)
    ax.ticklabel_format(style='scientific', axis='both', scilimits=(0, 0))
    for spine in ax.spines.values():
        spine.set_color('#444444')

    title_text = ax.text(
        0.5, 0.98, 'N-Body Orbital Simulation — Sun · Earth · Moon',
        transform=ax.transAxes, ha='center', va='top',
        color='white', fontsize=15, fontweight='bold'
    )
    info_text = ax.text(
        0.02, 0.04, '', transform=ax.transAxes,
        ha='left', va='bottom', color='white', fontsize=10,
        bbox=dict(boxstyle='round,pad=0.4', facecolor='#1e1e1e', alpha=0.85,
                  edgecolor='#555555')
    )

    TRAIL = 120   # frames of trail to show

    markers, trails = [], []
    for pid in particle_ids:
        b = bodies[pid]
        mass = b['mass']
        if mass > 1e29:          # Sun
            sz, mk = 18, '*'
        elif mass > 1e24:        # Earth
            sz, mk = 11, 'o'
        else:                    # Moon
            sz, mk = 6, 'o'

        m, = ax.plot([], [], mk, color=b['color'], markersize=sz,
                     markeredgecolor='white', markeredgewidth=0.6,
                     label=b['name'], zorder=5)
        t, = ax.plot([], [], '-', color=b['color'], alpha=0.45, linewidth=1.2, zorder=4)
        markers.append(m)
        trails.append(t)

    ax.legend(loc='upper right', facecolor='#1e1e1e', edgecolor='#555555',
              labelcolor='white', fontsize=11, framealpha=0.9)

    def init():
        for m in markers: m.set_data([], [])
        for t in trails:  t.set_data([], [])
        info_text.set_text('')
        return markers + trails + [info_text]

    def animate(frame):
        day  = frame
        year = day / 365.25
        info_text.set_text(f'Day {day:>4d}   Year {year:.3f}')
        for i, pid in enumerate(particle_ids):
            b = bodies[pid]
            if frame >= len(b['x']): continue
            markers[i].set_data([b['x'][frame]], [b['y'][frame]])
            s = max(0, frame - TRAIL)
            trails[i].set_data(b['x'][s:frame + 1], b['y'][s:frame + 1])
        return markers + trails + [info_text]

    anim = animation.FuncAnimation(
        fig, animate, init_func=init,
        frames=n_frames, interval=25, blit=True, repeat=True
    )

    print(f"Saving animation ({n_frames} frames) → {out_path}")
    print("  This may take 30–60 seconds ...")
    try:
        anim.save(out_path, writer='pillow', fps=30)
        print(f"  Saved: {out_path}")
    except Exception as e:
        print(f"  WARNING: Could not save GIF: {e}")
    plt.close(fig)


# ═════════════════════════════════════════════════════════════════════════════
# 3. Energy conservation plot
# ═════════════════════════════════════════════════════════════════════════════
def plot_energy(energy_csv_path, out_path):
    if not os.path.exists(energy_csv_path):
        print("No energy_log.csv found — skipping energy plot.")
        return

    print(f"Loading energy data from {energy_csv_path} ...")
    days, ke, pe, total, frac_err = [], [], [], [], []
    with open(energy_csv_path, 'r') as f:
        for row in csv.DictReader(f):
            days.append(float(row['time_days']))
            ke.append(float(row['KE']))
            pe.append(float(row['PE']))
            total.append(float(row['total_energy']))
            frac_err.append(float(row['fractional_error']) * 100.0)  # → percent

    days      = np.array(days)
    ke        = np.array(ke)
    pe        = np.array(pe)
    total     = np.array(total)
    frac_err  = np.array(frac_err)

    fig, axes = plt.subplots(2, 1, figsize=(12, 8), sharex=True)
    fig.patch.set_facecolor('#111111')
    plt.suptitle('Energy Conservation — Leapfrog Integrator (1 Year, dt = 1 day)',
                 color='white', fontsize=14, fontweight='bold', y=0.98)

    # Top panel: KE, PE, Total
    ax0 = axes[0]
    ax0.set_facecolor('#0d0d0d')
    ax0.plot(days, ke,    color='#FF6B6B', linewidth=1.0, alpha=0.85, label='Kinetic Energy (KE)')
    ax0.plot(days, pe,    color='#4ECDC4', linewidth=1.0, alpha=0.85, label='Potential Energy (PE)')
    ax0.plot(days, total, color='#FFE66D', linewidth=1.5, alpha=0.95, label='Total Energy (KE+PE)')
    ax0.set_ylabel('Energy (J)', color='white', fontsize=11)
    ax0.tick_params(colors='white')
    ax0.ticklabel_format(style='scientific', axis='y', scilimits=(0, 0))
    ax0.legend(facecolor='#1e1e1e', edgecolor='#555555', labelcolor='white',
               fontsize=10, framealpha=0.9)
    ax0.grid(True, alpha=0.2, linestyle='--', color='gray')
    for spine in ax0.spines.values(): spine.set_color('#333333')

    # Bottom panel: fractional energy error
    ax1 = axes[1]
    ax1.set_facecolor('#0d0d0d')
    ax1.plot(days, frac_err, color='#A8E6CF', linewidth=1.2)
    ax1.axhline(0, color='white', linewidth=0.6, linestyle='--', alpha=0.5)
    ax1.set_xlabel('Time (days)', color='white', fontsize=11)
    ax1.set_ylabel('δE/E₀ (%)', color='white', fontsize=11)
    ax1.tick_params(colors='white')
    ax1.grid(True, alpha=0.2, linestyle='--', color='gray')
    for spine in ax1.spines.values(): spine.set_color('#333333')

    max_err = np.abs(frac_err).max()
    ax1.set_title(f'Max |δE/E₀| = {max_err:.4f} %  — '
                  f'{"GOOD (< 0.5%)" if max_err < 0.5 else "WARNING: large drift"}',
                  color='#A8E6CF' if max_err < 0.5 else '#FF6B6B',
                  fontsize=10)

    plt.tight_layout()
    plt.savefig(out_path, dpi=120, facecolor=fig.get_facecolor())
    print(f"  Saved: {out_path}")
    plt.close(fig)


# ═════════════════════════════════════════════════════════════════════════════
# 4. Main
# ═════════════════════════════════════════════════════════════════════════════
if __name__ == '__main__':
    if not os.path.exists(ORBIT_CSV):
        print(f"ERROR: {ORBIT_CSV} not found. Run the simulation first (./nbody_sim).")
        sys.exit(1)

    bodies, particle_ids, n_frames = load_orbits(ORBIT_CSV)
    make_orbit_animation(bodies, particle_ids, n_frames, OUT_GIF)
    plot_energy(ENERGY_CSV, OUT_ENERGY)

    print("\nDone.")
    print(f"  Orbit animation : {OUT_GIF}")
    print(f"  Energy plot     : {OUT_ENERGY}")
