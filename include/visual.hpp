#ifndef VISUAL_HPP
#define VISUAL_HPP

#include <sstream>

#include "geometry.hpp"

/*
 * A wrapper for a painter that can draw primitives on the plane.
 *
 * Those primitives are: lines, Bezier curves, rectangles, text.
 */
class Painter {

protected:
    std::string path;

public:
    Painter() {};
    Painter(std::string path) {};
    virtual std::string getString() = 0;

    /* This method should be called in the end of drawing process. */
    virtual void end() = 0;

    /* Draw line between two points. */
    virtual void line(Vector point1, Vector point2, std::string settings) = 0;

    /* Draw cubic Bezier curve (with 2 control points). */
    virtual void curve(
        Vector point1,
        Vector point2,
        Vector point3,
        Vector point4,
        std::string settings)
        = 0;

    /* Draw text. */
    virtual void text(Vector center, std::string text, std::string settings)
        = 0;

    /* Draw axes aligned rectangle. */
    virtual void rectangle(Vector point1, Vector point2, std::string settings)
        = 0;
};

/* Write TikZ code of graphical primitives. */
class TikzPainter : public Painter {

    /* Stream to write TikZ code to. */
    std::stringstream stream;

public:
    TikzPainter(std::string path);
    std::string getString();
    void end();
    void line(Vector point1, Vector point2, std::string settings);
    void curve(
        Vector point1,
        Vector point2,
        Vector point3,
        Vector point4,
        std::string settings);
    void text(Vector center, std::string text, std::string settings);
    void rectangle(Vector point1, Vector point2, std::string settings);
};

#endif
