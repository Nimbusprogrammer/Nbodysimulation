cmake_minimum_required(VERSION 3.10)
project(NBodySimulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
        src/main.cpp
)

add_executable(nbody_sim ${SOURCES})
target_include_directories(nbody_sim PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test executable for Vec3
add_executable(test_vec3 tests/TestVec3.cpp)
target_include_directories(test_vec3 PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test executable for Particle
add_executable(test_particle tests/TestParticle.cpp)
target_include_directories(test_particle PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test executable for Logger
add_executable(test_logger tests/TestLogger.cpp)
target_include_directories(test_logger PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test for Integrator
add_executable(test_integrator tests/TestIntegrator.cpp)
target_include_directories(test_integrator PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Enable testing
enable_testing()
add_test(NAME Vec3Test COMMAND test_vec3)
add_test(NAME ParticleTest COMMAND test_particle)
add_test(NAME LoggerTest COMMAND test_logger)
add_test(NAME IntegratorTest COMMAND test_integrator)
