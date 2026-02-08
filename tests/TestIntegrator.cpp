#include <iostream>
#include <cassert>
#include "core/Particle.h"
#include "integrators/ForwardEuler.h"

void testConstantVelocity() {
    // 1. Setup: Particle with NO force
    Particle p(0, Vec3(0,0,0), Vec3(5,0,0), 1.0); // Moving at 5 m/s
    std::vector<Particle> particles = { p };

    // 2. Integrator
    ForwardEuler integrator;

    // 3. Step forward 1 second
    integrator.integrate(particles, 1.0);

    // 4. Verify
    // Pos should be 0 + 5*1 = 5
    // Vel should be 5 + 0 = 5
    assert(particles[0].position.x == 5.0);
    assert(particles[0].velocity.x == 5.0);

    std::cout << "[PASS] Constant Velocity Test\n";
}

void testConstantAcceleration() {
    // 1. Setup: Particle with Mass 2kg, Initial Vel 0
    Particle p(1, Vec3(0,0,0), Vec3(0,0,0), 2.0);

    // Apply Force of 4 Newtons in X direction
    p.force = Vec3(4, 0, 0);

    std::vector<Particle> particles = { p };

    // 2. Integrator
    ForwardEuler integrator;

    // 3. Step forward 2 seconds
    // Note: Euler is a linear approximation, so doing one big step of 2.0s
    // vs two steps of 1.0s might differ slightly in other integrators,
    // but for single-step verification:
    integrator.integrate(particles, 2.0);

    // 4. Verify
    // F=4, m=2 -> a=2.
    // Pos = 0 + 0*2 = 0 (Forward Euler uses OLD velocity)
    // Vel = 0 + 2*2 = 4

    // Note: This highlights a flaw in Forward Euler (Pos doesn't account for accel this frame).
    // But we test that the CODE does what we told it to do.
    assert(particles[0].position.x == 0.0);
    assert(particles[0].velocity.x == 4.0);

    // 5. Verify Force Reset
    // The integrator MUST reset force to 0 after usage.
    assert(particles[0].force.x == 0.0);

    std::cout << "[PASS] Constant Acceleration Test\n";
}

int main() {
    testConstantVelocity();
    testConstantAcceleration();
    std::cout << "=== Integrator Tests Passed ===\n";
    return 0;
}
