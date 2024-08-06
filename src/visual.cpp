#include <iostream>
#include <sstream>
#include <string>

#include "geometry.hpp"
#include "visual.hpp"

TikzPainter::TikzPainter(std::string path) {
}

std::string TikzPainter::getString() {
    return stream.str();
}

void TikzPainter::end() {
}

/* Draw line between two points. */
void TikzPainter::line(Vector point1, Vector point2, std::string settings) {

    stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
           << ") -- " << "(" << point2.x << ", " << point2.y << ");"
           << std::endl;
}

/* Draw cubic Bezier curve (with 2 control points). */
void TikzPainter::curve(
    Vector point1,
    Vector point2,
    Vector point3,
    Vector point4,
    std::string settings) {

    stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
           << ") .. " << "controls (" << point2.x << ", " << point2.y << ") "
           << "and (" << point3.x << ", " << point3.y << ") .. "
           << "(" << point4.x << ", " << point4.y << ");" << std::endl;
}

/* Draw text. */
void TikzPainter::text(Vector center, std::string text, std::string settings) {
    stream << "\\node[" << settings << "] at (" << center.x << ", " << center.y
           << ") {" << text << "};" << std::endl;
}

/* Draw axes aligned rectangle. */
void TikzPainter::rectangle(
    Vector point1, Vector point2, std::string settings) {
    stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
           << ") rectangle (" << point2.x << ", " << point2.y << ");"
           << std::endl;
}
