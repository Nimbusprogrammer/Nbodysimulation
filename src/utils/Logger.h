#pragma once
#include <vector>
#include <fstream>
#include "../core/Particle.h"

inline void writeHeader(std::ofstream& file)
{
    file << "time,id,mass,x,y,z,vx,vy,vz\n";
}

inline void logState(const std::vector<Particle>& particles, double time, std::ofstream& file)
{
    for (const auto& p : particles)
    {
        file << time << ","
             << p.id << ","
             << p.mass << ","
             << p.position.x << ","
             << p.position.y << ","
             << p.position.z << ","
             << p.velocity.x << ","
             << p.velocity.y << ","
             << p.velocity.z << "\n";
    }
}
