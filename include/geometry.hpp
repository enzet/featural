#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

/* Checks if two floating point numbers are approximately equal. */
bool equals(float x, float y);

/* Simple 2D vector. */
class Vector {

public:
    float x;
    float y;

    Vector();
    Vector(float x, float y);
    Vector operator*(float p);
    Vector operator+(Vector other);
    Vector operator-(Vector other);

    /* Checks if two vectors approximately equal. */
    bool operator==(Vector other);

    /* Check whether vectors are grid aligned and parallel. */
    bool isGridParallelTo(Vector other);

    /* Check whether vectors are grid aligned and codirected. */
    bool isGridCodirectedTo(Vector other);

    /* Check whether vertors are orthogonal. */
    bool isOrthogonalTo(Vector other);
};

#endif
