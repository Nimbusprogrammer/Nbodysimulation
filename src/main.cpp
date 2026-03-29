// src/main.cpp
// N-Body Gravitational Simulation — Sun + Earth + Moon
//
// Month 1 complete simulation:
//   - 3 bodies: Sun, Earth, Moon
//   - Leapfrog (symplectic) integrator for accurate long-term orbits
//   - 1 year simulation, 1-day timestep
//   - Output: data/output.csv (positions + velocities at each step)
//
// Physical values (SI units):
//   G        = 6.674×10⁻¹¹  N m² kg⁻²
//   Sun mass  = 1.989×10³⁰  kg
//   Earth mass= 5.972×10²⁴  kg
//   Moon mass = 7.342×10²²  kg
//   1 AU      = 1.496×10¹¹  m   (Earth-Sun distance)
//   Moon dist = 3.844×10⁸   m   (Earth-Moon distance)
//   Earth vel = 29 780       m/s (tangential, for circular orbit around Sun)
//   Moon vel  = 1 022        m/s (tangential relative to Earth, circular orbit)

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "core/Particle.h"
#include "dynamics/Gravity.h"
#include "integrators/Leapfrog.h"
#include "utils/Energy.h"
#include "utils/Logger.h"

int main() {
    // ── Simulation parameters ─────────────────────────────────────────────
    constexpr double G         = 6.67430e-11;
    constexpr double SOFTENING = 1.0e3;          // metres; prevents r=0 blow-up
    constexpr double DT        = 60.0 * 60.0 * 24.0;              // 1 day [s]
    constexpr double TOTAL     = 365.0 * 24.0 * 60.0 * 60.0;      // 1 year [s]
    constexpr int    STEPS     = static_cast<int>(TOTAL / DT);     // ≈ 365

    // ── Build the three-body system ───────────────────────────────────────
    // Coordinate system: Sun at origin, Earth along +x axis at t=0,
    // all bodies in the z=0 plane.

    std::vector<Particle> particles;

    // Sun — particle 0
    // Nearly stationary; will drift very slightly due to Earth + Moon gravity.
    particles.emplace_back(
        0,
        Vec3(0.0, 0.0, 0.0),   // position [m]
        Vec3(0.0, 0.0, 0.0),   // velocity [m/s]
        1.989e30                // mass [kg]
    );

    // Earth — particle 1
    // Placed at 1 AU along +x; velocity tangential (+y) for a circular orbit.
    particles.emplace_back(
        1,
        Vec3(1.496e11, 0.0, 0.0),
        Vec3(0.0, 29780.0, 0.0),
        5.972e24
    );

    // Moon — particle 2
    // Placed 3.844×10⁸ m ahead of Earth along +x (Earth-Moon distance).
    // Velocity = Earth's velocity + Moon's orbital velocity around Earth (+y).
    // Moon orbital velocity: v = sqrt(G * M_earth / r_moon) ≈ 1022 m/s
    particles.emplace_back(
        2,
        Vec3(1.496e11 + 3.844e8, 0.0, 0.0),
        Vec3(0.0, 29780.0 + 1022.0, 0.0),
        7.342e22
    );

    // ── Integrator ────────────────────────────────────────────────────────
    Leapfrog integrator;

    // ── Output files ──────────────────────────────────────────────────────
    std::filesystem::create_directories("data");

    std::ofstream orbitFile("data/output.csv");
    if (!orbitFile.is_open()) {
        std::cerr << "ERROR: Could not open data/output.csv\n";
        return 1;
    }

    std::ofstream energyFile("data/energy_log.csv");
    if (!energyFile.is_open()) {
        std::cerr << "ERROR: Could not open data/energy_log.csv\n";
        return 1;
    }
    energyFile << "step,time_days,KE,PE,total_energy,fractional_error\n";

    // ── Initial state ─────────────────────────────────────────────────────
    // Compute forces at t=0 (Leapfrog needs them for its first half-kick).
    computeGravityForces(particles, G, SOFTENING);
    const double E0 = computeTotalEnergy(particles, G);

    double time = 0.0;
    writeHeader(orbitFile);
    logState(particles, time, orbitFile);

    // Log initial energy
    {
        const double ke = computeKineticEnergy(particles);
        const double pe = computePotentialEnergy(particles, G);
        energyFile << "0," << "0," << ke << "," << pe << ","
                   << (ke + pe) << "," << 0.0 << "\n";
    }

    // ── Main simulation loop ──────────────────────────────────────────────
    std::cout << "Running Sun-Earth-Moon simulation for 1 year (" << STEPS << " steps)...\n";

    for (int step = 0; step < STEPS; ++step) {
        // Leapfrog: forces were computed at the END of the previous iteration
        // (or at t=0 above). integrate() uses them, advances state, resets forces.
        integrator.integrate(particles, DT);
        time += DT;

        // Recompute forces at the new positions for the next step.
        computeGravityForces(particles, G, SOFTENING);

        logState(particles, time, orbitFile);

        // Log energy every day (every step in this case)
        const double ke  = computeKineticEnergy(particles);
        const double pe  = computePotentialEnergy(particles, G);
        const double E   = ke + pe;
        const double dEE = fractionalEnergyError(E, E0);
        energyFile << (step + 1) << ","
                   << (time / 86400.0) << ","    // convert s → days
                   << ke << "," << pe << "," << E << "," << dEE << "\n";
    }

    // ── Summary ───────────────────────────────────────────────────────────
    const double Efinal = computeTotalEnergy(particles, G);
    const double drift  = fractionalEnergyError(Efinal, E0);

    std::cout << "Simulation complete.\n";
    std::cout << "  Orbit data  → data/output.csv      (" << STEPS + 1 << " rows)\n";
    std::cout << "  Energy log  → data/energy_log.csv  (" << STEPS + 1 << " rows)\n";
    std::printf("  Energy drift: δE/E₀ = %.6f %%\n", drift * 100.0);

    return 0;
}
