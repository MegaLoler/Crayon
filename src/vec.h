#pragma once

#include <cmath>

// 3d vector for positioning and colors
struct Vec {
    double x;
    double y;
    double z;
    Vec (double x = 0, double y = 0, double z = 0) : x (x), y (y), z (z) {}
    Vec pow (double scalar) {
        return Vec (::pow(x, scalar), ::pow(y, scalar), ::pow(z, scalar));
    }
    Vec operator + (Vec v) {
        return Vec (x + v.x, y + v.y, z + v.z);
    }
    Vec operator - () {
        return Vec (-x, -y, -z);
    }
    Vec operator - (Vec v) {
        return Vec (x - v.x, y - v.y, z - v.z);
    }
    Vec operator * (Vec v) {
        return Vec (x * v.x, y * v.y, z * v.z);
    }
    Vec operator / (Vec v) {
        return Vec (x / v.x, y / v.y, z / v.z);
    }
    Vec operator * (double scalar) {
        return Vec (x * scalar, y * scalar, z * scalar);
    }
    Vec operator / (double scalar) {
        return Vec (x / scalar, y / scalar, z / scalar);
    }
    double dot (Vec v) {
        Vec vn = v.normal ();
        Vec n = normal ();
        return n.x * vn.x + n.y * vn.y + n.z * vn.z;
    }
    double distance () {
        return sqrt (x * x + y * y + z * z);
    }
    Vec normal () {
        return *this / this->distance ();
    }
};
