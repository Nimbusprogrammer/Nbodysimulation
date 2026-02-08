#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "core/Particle.h"
#include "dynamics/Gravity.h"
#include "integrators/ForwardEuler.h"
#include "utils/Logger.h"

int main()
{
    const double G = 6.67430e-11;
    const double dt = 60.0 * 60.0 * 24.0; // 1 day
    const double totalTime = 365.0 * 24.0 * 60.0 * 60.0;
    const int steps = static_cast<int>(totalTime / dt);

    std::vector<Particle> particles;
    particles.emplace_back(0, Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0), 1.989e30);
    particles.emplace_back(1, Vec3(1.496e11, 0.0, 0.0), Vec3(0.0, 29780.0, 0.0), 5.972e24);

    ForwardEuler integrator;

    std::filesystem::create_directories("data");
    std::ofstream file("data/output.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open data/output.csv\n";
        return 1;
    }

    double time = 0.0;
    writeHeader(file);
    logState(particles, time, file);

    for (int step = 0; step < steps; ++step) {
        computeGravityForces(particles, G, 1.0e3);
        integrator.integrate(particles, dt);
        time += dt;
        logState(particles, time, file);
    }

    std::cout << "Simulation complete. Output written to data/output.csv\n";
    return 0;
}
