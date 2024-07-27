#ifndef VISUAL_HPP
#define VISUAL_HPP

#include <sstream>

#include "geometry.hpp"

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

/*
 * Wrapper for drawing Tikz elements.
 */
class TikzPainter : public Painter {

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
