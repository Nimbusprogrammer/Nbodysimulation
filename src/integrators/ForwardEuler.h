// src/integrators/ForwardEuler.h
#ifndef FORWARD_EULER_H
#define FORWARD_EULER_H

#include "TimeIntegrator.h"

class ForwardEuler : public TimeIntegrator {
public:
    void integrate(std::vector<Particle>& particles, double dt) override {
        for (auto& p : particles) {
            // 1. Update Position: r = r + v * dt
            p.position += p.velocity * dt;

            // 2. Calculate Acceleration: a = F / m
            // (Check mass to avoid division by zero)
            if (p.mass > 0.0) {
                Vec3 acceleration = p.force / p.mass;

                // 3. Update Velocity: v = v + a * dt
                p.velocity += acceleration * dt;
            }

            // 4. CRITICAL: Reset force for the next step.
            // If we don't do this, gravity will accumulate infinitely!
            p.force = Vec3(0.0, 0.0, 0.0);
        }
    }
};

<<<<<<< HEAD
#endif // FORWARD_EULER_H
=======
#endif // FORWARD_EULER_H
>>>>>>> copilot/complete-n-body-simulation-engine
