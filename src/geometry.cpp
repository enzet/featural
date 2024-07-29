#include <cmath>

#include "geometry.hpp"

#define PRECISION 1e-5f

bool equals(float x, float y) {
    return std::fabs(x - y) < PRECISION;
}

Vector::Vector() {
    this->x = 0.0f;
    this->y = 0.0f;
}

Vector::Vector(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector Vector::operator*(float p) {
    return Vector(x * p, y * p);
}

Vector Vector::operator+(Vector other) {
    return Vector(x + other.x, y + other.y);
}

Vector Vector::operator-(Vector other) {
    return Vector(x - other.x, y - other.y);
}

bool Vector::operator==(Vector other) {
    return x == other.x and y == other.y;
}

bool Vector::isGridParallelTo(Vector other) {
    return (x == 0 and other.x == 0) or (y == 0 and other.y == 0);
}

bool Vector::isGridCodirectedTo(Vector other) {
    return (x == 0 and other.x == 0 and y * other.y >= 0)
        or (y == 0 and other.y == 0 and x * other.x >= 0);
}

bool Vector::isOrthogonalTo(Vector other) {
    return equals(x * other.x + y * other.y, 0);
}
