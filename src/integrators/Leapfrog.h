// src/integrators/Leapfrog.h
#ifndef LEAPFROG_H
#define LEAPFROG_H

#include "TimeIntegrator.h"

// Leapfrog (Störmer-Verlet) Integrator
//
// WHY THIS IS BETTER THAN FORWARD EULER:
//   Forward Euler is NOT symplectic — it doesn't preserve the geometric structure
//   of Hamiltonian mechanics. Energy drifts monotonically upward over time,
//   meaning orbits slowly spiral outward. This is unphysical.
//
//   Leapfrog IS symplectic — it conserves a slightly modified Hamiltonian exactly.
//   Total energy oscillates very slightly around its true value but never drifts.
//   Orbits stay closed over millions of steps. This is the gold standard for
//   gravitational simulation.
//
// HOW IT WORKS (Kick-Drift form):
//   particle.velocity stores v at half-integer timestep: v[n - 1/2]
//   particle.position stores r at integer timestep:      r[n]
//
//   Each call to integrate() receives forces F(r[n]) and does:
//     1. Full kick:  v[n+1/2] = v[n-1/2] + a[n] * dt      (a = F/m)
//     2. Full drift: r[n+1]   = r[n]     + v[n+1/2] * dt
//
//   The positions and velocities "leap over" each other — hence the name.
//
// INITIALIZATION:
//   The very first call converts v[0] → v[-1/2] via a half-kick backward,
//   then proceeds with the standard kick-drift step. This is transparent to
//   the caller — the main loop is identical to ForwardEuler.
//
// INTERFACE COMPATIBILITY:
//   Uses the identical pipeline as ForwardEuler:
//     computeGravityForces(particles, G, softening);
//     integrator.integrate(particles, dt);
class Leapfrog : public TimeIntegrator {
    bool m_initialized = false;

public:
    void integrate(std::vector<Particle>& particles, double dt) override {
        for (auto& p : particles) {
            if (p.mass <= 0.0) {
                p.force = Vec3(0.0, 0.0, 0.0);
                continue;
            }

            const Vec3 a = p.force / p.mass;

            if (!m_initialized) {
                // One-time initialization: shift v[0] → v[-dt/2] with a half-kick
                // backward so that on this first step the full kick lands at v[+dt/2].
                // Net effect: v goes from v[0] to v[+dt/2] = v[0] + a[0]*dt/2.
                p.velocity -= a * (0.5 * dt);
            }

            // Full kick: v[n-1/2] → v[n+1/2]
            p.velocity += a * dt;

            // Full drift: r[n] → r[n+1]
            p.position += p.velocity * dt;

            p.force = Vec3(0.0, 0.0, 0.0);
        }

        m_initialized = true;
    }
};

#endif // LEAPFROG_H
