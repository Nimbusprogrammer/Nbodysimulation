#ifndef TIME_INTEGRATOR_H
#define TIME_INTEGRATOR_H

#include <vector>
#include "../core/Particle.h"

// Abstract Base Class
class TimeIntegrator {
public:
    virtual ~TimeIntegrator() = default;
    // Pure virtual function: forces children to implement this.
    // particles: The list of bodies to move
    // dt: The small slice of time (e.g., 0.01 seconds)
    virtual void integrate(std::vector<Particle>& particles, double dt) = 0;
};

#endif // TIME_INTEGRATOR_H