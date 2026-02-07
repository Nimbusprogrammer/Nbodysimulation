#ifndef PARTICLE_H
#define PARTICLE_H
#include "Vec3.h"
struct Particle
{
    Vec3 position, velocity, force;
    double mass;
    int id;
    Particle(int id, const Vec3& pos, const Vec3& vel, double m)
        : id(id), position(pos), velocity(vel), mass(m), force(0.0, 0.0, 0.0) {}
};
#endif