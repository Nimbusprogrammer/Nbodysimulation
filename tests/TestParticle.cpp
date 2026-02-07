#include <cassert>
#include <cmath>
#include <iostream>
#include "../src/core/Particle.h"

void testParticleConstruction() {
    Vec3 pos(1.0, 2.0, 3.0);
    Vec3 vel(0.1, 0.2, 0.3);
    Particle p(1, pos, vel, 5.0);

    assert(p.id == 1);
    assert(p.position.x == 1.0);
    assert(p.position.y == 2.0);
    assert(p.position.z == 3.0);
    assert(p.velocity.x == 0.1);
    assert(p.velocity.y == 0.2);
    assert(p.velocity.z == 0.3);
    assert(p.mass == 5.0);
    // Force should be initialized to zero
    assert(p.force.x == 0.0);
    assert(p.force.y == 0.0);
    assert(p.force.z == 0.0);

    std::cout << "[PASS] Particle construction\n";
}

void testForceAccumulation() {
    Particle p(0, Vec3(0,0,0), Vec3(0,0,0), 1.0);

    // Simulate forces from multiple particles
    Vec3 f1(1.0, 0.0, 0.0);
    Vec3 f2(0.0, 2.0, 0.0);
    Vec3 f3(0.0, 0.0, 3.0);

    p.force = p.force + f1;
    p.force = p.force + f2;
    p.force = p.force + f3;

    assert(p.force.x == 1.0);
    assert(p.force.y == 2.0);
    assert(p.force.z == 3.0);

    std::cout << "[PASS] Force accumulation\n";
}

void testMultipleParticles() {
    Particle sun(0, Vec3(0,0,0), Vec3(0,0,0), 1.989e30);      // Sun mass in kg
    Particle earth(1, Vec3(1.496e11,0,0), Vec3(0,29780,0), 5.972e24);  // Earth at 1 AU

    assert(sun.id == 0);
    assert(earth.id == 1);
    assert(sun.mass > earth.mass);

    std::cout << "[PASS] Multiple particles (Sun-Earth)\n";
}

int main() {
    std::cout << "=== Particle Tests ===\n";

    testParticleConstruction();
    testForceAccumulation();
    testMultipleParticles();

    std::cout << "=== All Particle tests passed! ===\n";
    return 0;
}

