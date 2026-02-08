#ifndef GRAVITY_H
#define GRAVITY_H

#include <cmath>
#include <vector>
#include "core/Particle.h"

inline void computeGravityForces(std::vector<Particle>& particles, double G, double softening = 1.0e-3)
{
    const double softeningSq = softening * softening;
    const std::size_t n = particles.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            Vec3 r = particles[j].position - particles[i].position;
            double distSq = r.magSq() + softeningSq;
            double dist = std::sqrt(distSq);
            double forceMag = (G * particles[i].mass * particles[j].mass) / distSq;
            Vec3 force = r * (forceMag / dist);

            particles[i].force += force;
            particles[j].force -= force;
        }
    }
}

#endif // GRAVITY_H
