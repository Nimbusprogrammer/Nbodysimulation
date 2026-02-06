#include <iostream>
#include <cassert>
#include <cmath>
#include "../src/core/Vec3.h"

int main()
{
    //Test construction first
    Vec3 v1(1.0,2.0,3.0);
    assert(v1.x == 1.0);
    assert(v1.y == 2.0);
    assert(v1.z == 3.0);
    //Test Addition
    Vec3 v2(4.0,5.0,6.0);
    Vec3 v3 = v1 + v2;
    assert(v3.x == 5.0);
    assert(v3.y == 7.0);
    assert(v3.z == 9.0);
    //Test Subtraction
    Vec3 v4 = v2 - v1;
    assert(v4.x == 3.0);
    assert(v4.y == 3.0);
    assert(v4.z == 3.0);
    //Testing scalar multiplication
    v4 =  v3*2.0;
    assert(v4.x == 10.0);
    assert(v4.y == 14.0);
    assert(v4.z == 18.0);
    //Testing scalar division
    Vec3 v5 = v3/2.0;
    assert(v5.x == 2.5);
    assert(v5.y == 3.5);
    assert(v5.z == 4.5);
    //Testing magnitude (using epsilon for floating-point comparison)
    double epsilon = 1e-9;
    assert(std::abs(v3.mag() - std::sqrt(155.0)) < epsilon);
    //Testing square of magnitude
    assert(v3.magSq() == 155.0);
    //Testing *= operator
    Vec3 v6(2.0, 3.0, 4.0);
    v6 *= 2.0;
    assert(v6.x == 4.0);
    assert(v6.y == 6.0);
    assert(v6.z == 8.0);

    //Testing /= operator
    Vec3 v7(10.0, 20.0, 30.0);
    v7 /= 2.0;
    assert(v7.x == 5.0);
    assert(v7.y == 10.0);
    assert(v7.z == 15.0);

    //Testing += operator
    Vec3 v8(1.0, 2.0, 3.0);
    v8 += Vec3(1.0, 1.0, 1.0);
    assert(v8.x == 2.0);
    assert(v8.y == 3.0);
    assert(v8.z == 4.0);

    //Testing -= operator
    Vec3 v9(5.0, 5.0, 5.0);
    v9 -= Vec3(1.0, 2.0, 3.0);
    assert(v9.x == 4.0);
    assert(v9.y == 3.0);
    assert(v9.z == 2.0);

    std::cout << "All Vec3 tests passed!" << std::endl;
    return 0;
}