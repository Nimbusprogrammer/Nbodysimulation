cmake_minimum_required(VERSION 3.10)
project(NBodySimulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        src/main.cpp
)

add_executable(nbody_sim ${SOURCES})

# Test executable for Vec3
add_executable(test_vec3 tests/TestVec3.cpp)
target_include_directories(test_vec3 PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Enable testing
enable_testing()
add_test(NAME Vec3Test COMMAND test_vec3)
