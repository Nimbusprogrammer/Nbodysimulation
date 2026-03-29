// src/utils/Energy.h
#pragma once

#include <vector>
#include <cmath>
#include "core/Particle.h"

// ─────────────────────────────────────────────────────────────────────────────
// Energy diagnostics for N-body simulations
//
// WHY ENERGY MATTERS:
//   Total mechanical energy (KE + PE) is a conserved quantity in Newtonian
//   gravity. If our integrator is correct, total energy should stay constant
//   over time. Drift = integrator error accumulating.
//
//   Forward Euler adds energy to the system every step → orbits spiral outward.
//   Leapfrog conserves a modified energy exactly → orbits stay closed.
//
// NOTE ON LEAPFROG VELOCITIES:
//   Leapfrog stores v at half-integer timesteps (v[n+1/2]), while positions are
//   at integer steps (r[n]). The kinetic energy computed here uses the stored
//   half-step velocity, which gives the "leapfrog shadow Hamiltonian" — a
//   quantity that oscillates very slightly but never drifts. This is the
//   expected and correct behaviour for a symplectic integrator.
// ─────────────────────────────────────────────────────────────────────────────

// Kinetic energy: KE = Σ  1/2 * m * |v|²
inline double computeKineticEnergy(const std::vector<Particle>& particles) {
    double ke = 0.0;
    for (const auto& p : particles) {
        ke += 0.5 * p.mass * p.velocity.magSq();
    }
    return ke;
}

// Gravitational potential energy: PE = Σ_{i<j}  -G * m_i * m_j / r_{ij}
// (No softening here — we want the true physical energy, not the softened one.)
inline double computePotentialEnergy(const std::vector<Particle>& particles, double G) {
    double pe = 0.0;
    const std::size_t n = particles.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            const Vec3 r = particles[j].position - particles[i].position;
            const double dist = r.mag();
            if (dist > 0.0) {
                pe -= G * particles[i].mass * particles[j].mass / dist;
            }
        }
    }
    return pe;
}

// Total mechanical energy: E = KE + PE
inline double computeTotalEnergy(const std::vector<Particle>& particles, double G) {
    return computeKineticEnergy(particles) + computePotentialEnergy(particles, G);
}

// Fractional energy error relative to initial energy:
//   δE/E₀ = (E(t) - E₀) / |E₀|
// A well-behaved integrator keeps this near zero. Forward Euler shows
// monotonic growth; Leapfrog shows bounded oscillation.
inline double fractionalEnergyError(double currentEnergy, double initialEnergy) {
    if (initialEnergy == 0.0) return 0.0;
    return (currentEnergy - initialEnergy) / std::abs(initialEnergy);
}
