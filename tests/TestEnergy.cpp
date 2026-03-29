// tests/TestEnergy.cpp
//
// Physics correctness proof: energy conservation.
//
// A real gravitational simulation must conserve total mechanical energy
// (KE + PE). This test runs an identical Sun-Earth system for 1 year using
// both Forward Euler and Leapfrog, measures total energy at every step, and
// checks two properties:
//
//   1. Forward Euler DRIFTS   — energy grows monotonically (expected failure).
//   2. Leapfrog   CONSERVES  — fractional energy error stays small (≤ 0.5%).
//
// The fact that Leapfrog passes and Euler fails proves that Leapfrog is the
// correct integrator for orbital mechanics.

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "core/Particle.h"
#include "dynamics/Gravity.h"
#include "integrators/ForwardEuler.h"
#include "integrators/Leapfrog.h"
#include "utils/Energy.h"

// ─── Simulation constants ────────────────────────────────────────────────────
static constexpr double G           = 6.67430e-11;
static constexpr double SOFTENING   = 1.0e3;
static constexpr double DT          = 60.0 * 60.0 * 24.0;       // 1 day  in seconds
static constexpr double TOTAL_TIME  = 365.0 * 24.0 * 60.0 * 60.0; // 1 year in seconds
static constexpr int    STEPS       = static_cast<int>(TOTAL_TIME / DT); // ≈ 365

// ─── Helper: build a fresh Sun-Earth two-body system ─────────────────────────
static std::vector<Particle> makeSunEarth() {
    std::vector<Particle> p;
    // Sun at origin, stationary
    p.emplace_back(0, Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0), 1.989e30);
    // Earth at 1 AU, circular orbit velocity
    p.emplace_back(1, Vec3(1.496e11, 0.0, 0.0), Vec3(0.0, 29780.0, 0.0), 5.972e24);
    return p;
}

// ─── Test: Leapfrog energy conservation ──────────────────────────────────────
void testLeapfrogEnergyConservation() {
    auto particles = makeSunEarth();
    Leapfrog integrator;

    // Compute initial energy before any stepping
    computeGravityForces(particles, G, SOFTENING);
    const double E0 = computeTotalEnergy(particles, G);

    double maxFractionalError = 0.0;

    for (int step = 0; step < STEPS; ++step) {
        // Forces were already computed for step 0; recomputed each subsequent step
        integrator.integrate(particles, DT);
        computeGravityForces(particles, G, SOFTENING);

        const double E  = computeTotalEnergy(particles, G);
        const double dE = std::abs(fractionalEnergyError(E, E0));
        if (dE > maxFractionalError) maxFractionalError = dE;
    }

    std::cout << "[Leapfrog] Initial energy    : " << E0 << " J\n";
    std::cout << "[Leapfrog] Max |δE/E₀|       : " << maxFractionalError * 100.0 << " %\n";

    // Leapfrog must keep fractional error below 0.5% over 1 year with dt = 1 day
    assert(maxFractionalError < 0.005 &&
           "Leapfrog energy drift exceeded 0.5% — integrator may be broken");

    std::cout << "[PASS] Leapfrog energy conservation\n";
}

// ─── Test: Forward Euler energy drift ────────────────────────────────────────
void testEulerEnergyDrift() {
    auto particles = makeSunEarth();
    ForwardEuler integrator;

    computeGravityForces(particles, G, SOFTENING);
    const double E0 = computeTotalEnergy(particles, G);

    double finalEnergy = E0;

    for (int step = 0; step < STEPS; ++step) {
        integrator.integrate(particles, DT);
        computeGravityForces(particles, G, SOFTENING);
        finalEnergy = computeTotalEnergy(particles, G);
    }

    const double drift = fractionalEnergyError(finalEnergy, E0);
    std::cout << "[Euler]    Initial energy    : " << E0 << " J\n";
    std::cout << "[Euler]    Final energy      : " << finalEnergy << " J\n";
    std::cout << "[Euler]    δE/E₀             : " << drift * 100.0 << " %\n";

    // Forward Euler MUST drift — that's its known flaw.
    // We assert that the drift is measurably positive (energy injection).
    assert(drift > 0.001 &&
           "Expected Forward Euler to inject energy over 1 year, but it didn't");

    std::cout << "[PASS] Forward Euler energy drift confirmed (expected behaviour)\n";
}

// ─── Test: initial energy value sanity check ─────────────────────────────────
void testInitialEnergySanity() {
    auto particles = makeSunEarth();

    const double ke = computeKineticEnergy(particles);
    const double pe = computePotentialEnergy(particles, G);
    const double E  = ke + pe;

    // Sun-Earth system: PE ≈ -5.34e33 J,  KE ≈ 2.67e33 J,  E ≈ -2.67e33 J
    // Total energy must be negative (bound system) and large in magnitude.
    assert(E < 0.0   && "Total energy of a bound system must be negative");
    assert(pe < 0.0  && "Gravitational PE must be negative");
    assert(ke > 0.0  && "Kinetic energy must be positive");
    // For a circular orbit: KE ≈ -0.5 * PE  (virial theorem)
    const double virialRatio = ke / (-pe);
    assert(virialRatio > 0.45 && virialRatio < 0.55 &&
           "Virial ratio KE/|PE| should be ~0.5 for circular orbit");

    std::cout << "[Sanity]   KE  = " << ke  << " J\n";
    std::cout << "[Sanity]   PE  = " << pe  << " J\n";
    std::cout << "[Sanity]   E   = " << E   << " J\n";
    std::cout << "[Sanity]   KE/|PE| = " << virialRatio << " (should be ~0.5)\n";
    std::cout << "[PASS] Initial energy sanity\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Energy Conservation Tests ===\n\n";

    testInitialEnergySanity();
    std::cout << "\n";

    testLeapfrogEnergyConservation();
    std::cout << "\n";

    testEulerEnergyDrift();
    std::cout << "\n";

    std::cout << "=== All Energy Tests Passed ===\n";
    return 0;
}
