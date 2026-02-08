#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "core/Particle.h"
#include "utils/Logger.h"

void testWriteHeader() {
    std::ofstream file("/tmp/test_header.csv");
    writeHeader(file);
    file.close();

    std::ifstream in("/tmp/test_header.csv");
    assert(in.is_open());
    std::string line;
    std::getline(in, line);
    assert(line == "time,id,mass,x,y,z,vx,vy,vz");
    in.close();

    std::cout << "[PASS] writeHeader\n";
}

void testLogState() {
    std::vector<Particle> particles;
    particles.emplace_back(0, Vec3(1.0, 2.0, 3.0), Vec3(0.1, 0.2, 0.3), 5.0);
    particles.emplace_back(1, Vec3(4.0, 5.0, 6.0), Vec3(0.4, 0.5, 0.6), 10.0);

    std::ofstream file("/tmp/test_log.csv");
    writeHeader(file);
    logState(particles, 0.0, file);
    file.close();

    std::ifstream in("/tmp/test_log.csv");
    assert(in.is_open());
    std::string line;

    std::getline(in, line);
    assert(line == "time,id,mass,x,y,z,vx,vy,vz");

    std::getline(in, line);
    assert(line == "0,0,5,1,2,3,0.1,0.2,0.3");

    std::getline(in, line);
    assert(line == "0,1,10,4,5,6,0.4,0.5,0.6");

    in.close();

    std::cout << "[PASS] logState\n";
}

void testLogStateMultipleTimes() {
    std::vector<Particle> particles;
    particles.emplace_back(0, Vec3(0, 0, 0), Vec3(1, 0, 0), 1.0);

    std::ofstream file("/tmp/test_log_multi.csv");
    writeHeader(file);
    logState(particles, 0.0, file);
    logState(particles, 1.0, file);
    file.close();

    std::ifstream in("/tmp/test_log_multi.csv");
    assert(in.is_open());
    std::string line;
    int count = 0;
    while (std::getline(in, line)) {
        count++;
    }
    assert(count == 3); // header + 2 data lines
    in.close();

    std::cout << "[PASS] logState multiple times\n";
}

int main() {
    std::cout << "=== Logger Tests ===\n";

    testWriteHeader();
    testLogState();
    testLogStateMultipleTimes();

    std::cout << "=== All Logger tests passed! ===\n";
    return 0;
}
