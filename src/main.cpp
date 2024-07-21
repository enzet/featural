#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

float DOUBLE_SIZE = 0.5;
float CURVE_SIZE = 0.5;
float CURVATURE = 0.6;
float PRECISION = 0.01;

bool equals(float x, float y) {
    return abs(x - y) < PRECISION;
}

class Vector {

public:
    float x;
    float y;

    Vector() {
        this->x = 0.0f;
        this->y = 0.0f;
    }

    Vector(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector operator*(float p) {
        return Vector(x * p, y * p);
    }

    Vector operator+(Vector other) {
        return Vector(x + other.x, y + other.y);
    }

    Vector operator-(Vector other) {
        return Vector(x - other.x, y - other.y);
    }

    bool operator==(Vector other) {
        return x == other.x and y == other.y;
    }

    /* Check whether vectors are grid alligned and parallel. */
    bool isGridParallelTo(Vector other) {
        return (x == 0 and other.x == 0) or (y == 0 and other.y == 0);
    }

    /* Check whether vectors are grid alligned and codirected. */
    bool isGridCodirectedTo(Vector other) {
        return (x == 0 and other.x == 0 and y * other.y >= 0)
            or (y == 0 and other.y == 0 and x * other.x >= 0);
    }

    /* Check whether vertors are orthogonal. */
    bool isOrthogonalTo(Vector other) {
        return equals(x * other.x + y * other.y, 0);
    }
};

class Painter {

public:
    virtual void end() = 0;
    virtual std::string getString() = 0;

    virtual void line(Vector point1, Vector point2, std::string settings) = 0;
    virtual void curve(
        Vector point1,
        Vector point2,
        Vector point3,
        Vector point4,
        std::string settings)
        = 0;
    virtual void text(Vector center, std::string text, std::string settings)
        = 0;
    virtual void rectangle(Vector point1, Vector point2, std::string settings)
        = 0;
};

/*
 * Wrapper for drawing Tikz elements.
 */
class TikzPainter : public Painter {

    /* Output stream. */
    std::stringstream stream;

public:
    TikzPainter() {
        stream << "\\documentclass[tikz]{standalone}" << std::endl;

        stream << "\\usepackage[T2A]{fontenc}" << std::endl;
        stream << "\\usepackage[utf8]{inputenc}" << std::endl;
        stream << "\\usepackage{tikz}" << std::endl;
        stream << "\\usepackage{fontspec}" << std::endl;
        stream << "\\setmainfont{Doulos SIL}" << std::endl;
        stream << "\\usetikzlibrary{positioning}" << std::endl;

        stream << "\\begin{document}" << std::endl;
        stream << "\\begin{tikzpicture}" << std::endl;
    }

    std::string getString() {
        return stream.str();
    };

    void end() {
        stream << "\\end{tikzpicture}" << std::endl;
        stream << "\\end{document}" << std::endl;
    }

    /* Draw line between two points. */
    void line(Vector point1, Vector point2, std::string settings) {

        stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
               << ") -- " << "(" << point2.x << ", " << point2.y << ");"
               << std::endl;
    }

    /* Draw cubic Bezier curve (with 2 control points). */
    void curve(
        Vector point1,
        Vector point2,
        Vector point3,
        Vector point4,
        std::string settings) {

        stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
               << ") .. " << "controls (" << point2.x << ", " << point2.y
               << ") " << "and (" << point3.x << ", " << point3.y << ") .. "
               << "(" << point4.x << ", " << point4.y << ");" << std::endl;
    }

    /* Draw text. */
    void text(Vector center, std::string text, std::string settings) {
        stream << "\\node[" << settings << "] at (" << center.x << ", "
               << center.y << ") {" << text << "};" << std::endl;
    }

    /* Draw axes aligned rectangle. */
    void rectangle(Vector point1, Vector point2, std::string settings) {
        stream << "\\draw[" << settings << "] (" << point1.x << ", " << point1.y
               << ") rectangle (" << point2.x << ", " << point2.y << ");"
               << std::endl;
    }
};

/*
 * Describes a graphical element: how and where to draw it.
 */
enum class ElementDescriptor {

    // Direction.
    Horizontal,
    Vertical,
    Slash,
    Backslash,

    // Position.
    Top,
    Bottom,
    Left,
    Right,
    Center,

    // Curvature.
    CurvedInwards,
    CurvedOutwards,
    PointedInwards,
    PointedOutwards,

    Double, // Number of lines.
    Short, // Width.

    Unknown
};

/*
 * Symbol element.
 *
 *     p1        p2
 *     *----*----*
 *
 *          ↑
 *     norm |
 *          *---->
 *           direction
 */
class Element {

    std::vector<ElementDescriptor> elementDescriptors;
    Vector indirectedNorm = Vector();
    float position = 0;

    Vector direction = Vector();

    Vector getNorm() {
        return indirectedNorm * position;
    }

    Vector getPoint1() {
        return getNorm() + direction;
    }

    Vector getPoint2() {
        return getNorm() - direction;
    }

    bool isDouble = false;
    bool isCurved = false;
    bool isPointed = false;
    bool isInwards = false;
    bool isDiagonal = false;

public:
    void add(ElementDescriptor elementDescriptor) {

        elementDescriptors.push_back(elementDescriptor);

        switch (elementDescriptor) {
        case ElementDescriptor::Horizontal:
            indirectedNorm = Vector(0.0f, 1.0f);
            direction = Vector(1.0f, 0.0f);
            break;
        case ElementDescriptor::Vertical:
            indirectedNorm = Vector(1.0f, 0.0f);
            direction = Vector(0.0f, 1.0f);
            break;
        case ElementDescriptor::Slash:
            indirectedNorm = Vector(1.0f, 1.0f);
            direction = Vector(1.0f, -1.0f);
            isDiagonal = true;
            break;
        case ElementDescriptor::Backslash:
            indirectedNorm = Vector(1.0f, -1.0f);
            direction = Vector(1.0f, 1.0f);
            isDiagonal = true;
            break;
        case ElementDescriptor::Center:
            position = 0;
            break;
        case ElementDescriptor::Right:
            position = 1;
            break;
        case ElementDescriptor::Left:
            position = -1;
            break;
        case ElementDescriptor::Top:
            position = 1;
            break;
        case ElementDescriptor::Bottom:
            position = -1;
            break;
        case ElementDescriptor::Double:
            isDouble = true;
            break;
        case ElementDescriptor::CurvedInwards:
            isCurved = true;
            isInwards = true;
            break;
        case ElementDescriptor::CurvedOutwards:
            isCurved = true;
            break;
        case ElementDescriptor::PointedInwards:
            isPointed = true;
            isInwards = true;
            break;
        case ElementDescriptor::PointedOutwards:
            isPointed = true;
            break;
        }
    }

    void draw(
        Painter* painter,
        Vector center,
        float size,
        Vector step,
        std::vector<Element> elements) {

        Vector a = getNorm();
        Vector b = direction;

        if (isCurved) {

            float m = isInwards ? 1 : -1;
            if (isInwards) {
                step = step + a * -CURVE_SIZE;
            }

            // Line.
            Vector start = step + b * (1 - CURVE_SIZE);
            Vector end = step - b * (1 - CURVE_SIZE);
            painter->line(
                center + start * size, center + end * size, "line cap=round");

            // Curve.
            Vector p = step + b;
            Vector p1 = p - b * CURVE_SIZE;
            Vector p2 = p - b * CURVE_SIZE * (1 - CURVATURE);
            Vector p3 = p + (a * CURVE_SIZE * (1 - CURVATURE)) * m;
            Vector p4 = p + (a * CURVE_SIZE) * m;
            painter->curve(
                center + p1 * size,
                center + p2 * size,
                center + p3 * size,
                center + p4 * size,
                "line cap=round");

            // Curve.
            p = step - b;
            p1 = p + b * CURVE_SIZE;
            p2 = p + b * CURVE_SIZE * (1 - CURVATURE);
            p3 = p + (a * CURVE_SIZE * (1 - CURVATURE)) * m;
            p4 = p + (a * CURVE_SIZE) * m;
            painter->curve(
                center + p1 * size,
                center + p2 * size,
                center + p3 * size,
                center + p4 * size,
                "line cap=round");

        } else if (isPointed) {

        } else { // Horizontal, vertical, or diagonal line.

            Vector p1 = step + b; // Start point.
            Vector p2 = step + b; // Curve point 1.
            Vector p3 = step - b; // Curve point 2.
            Vector p4 = step - b; // End point.

            // Check other elements.
            // TODO: ignore the element itself.
            for (Element element : elements) {

                // Other element is double.
                if (element.isDouble
                    and getNorm().isGridParallelTo(element.direction)
                    and element.position != 0) {

                    // Shift point.
                    if (isDiagonal) {
                        if (element.getPoint1() == p1
                            or element.getPoint2() == p1) {

                            p1 = p1 + element.getNorm() * -DOUBLE_SIZE;
                            p2 = p2 + element.getNorm() * -DOUBLE_SIZE;
                        }
                        if (element.getPoint1() == p4
                            or element.getPoint2() == p4) {

                            p4 = p4 + element.getNorm() * -DOUBLE_SIZE;
                            p3 = p3 + element.getNorm() * -DOUBLE_SIZE;
                        }
                    } else if (element.getNorm().isGridCodirectedTo(
                                   direction)) {
                        p1 = p1 - direction * DOUBLE_SIZE;
                        p2 = p2 - direction * DOUBLE_SIZE;
                    } else {
                        p4 = p4 + direction * DOUBLE_SIZE;
                        p3 = p3 + direction * DOUBLE_SIZE;
                    }
                }

                // Other element is curved.
                if (element.isCurved) {

                    // Shift point if element is no the edge, orthogonal to the
                    // curved element, and curved inwards.
                    if ((step.x == 1 or step.x == -1 or step.y == 1
                         or step.y == -1)
                        and getNorm().isGridParallelTo(element.direction)
                        and not element.isInwards) {
                        // Shift point.
                        if (element.getNorm().isGridCodirectedTo(direction)) {
                            p1 = p1 - direction * CURVE_SIZE;
                            p2 = p2 - direction * CURVE_SIZE;
                        } else {
                            p4 = p4 + direction * CURVE_SIZE;
                            p3 = p3 + direction * CURVE_SIZE;
                        }
                    }

                    // Shift and curve diagnoal elements.
                    if (isDiagonal) {
                        if (element.getPoint1() == p1
                            or element.getPoint2() == p1) {

                            if (not element.isInwards) {
                                p1 = p1 + element.getNorm() * -CURVE_SIZE;
                            }
                            p2 = p2 + element.getNorm() * -CURVE_SIZE * 3;
                        }
                        if (element.getPoint1() == p4
                            or element.getPoint2() == p4) {

                            p3 = p3 + element.getNorm() * -CURVE_SIZE * 3;
                            if (not element.isInwards) {
                                p4 = p4 + element.getNorm() * -CURVE_SIZE;
                            }
                        }
                    }
                }
            }
            painter->curve(
                center + p1 * size,
                center + p2 * size,
                center + p3 * size,
                center + p4 * size,
                "line cap=round");
        }
    }

    /*
     * Draw symbol element.
     */
    void draw(
        Painter* painter,
        Vector center,
        float size,
        std::vector<Element> elements) {

        Vector step = getNorm() * 0.5f;

        if (this->isDouble and position == 0) {
            draw(painter, center, size, getNorm() * 0.3f, elements);
            draw(painter, center, size, getNorm() * -1 * 0.3f, elements);
        } else {
            draw(painter, center, size, getNorm() * 1.0f, elements);
            if (this->isDouble) {
                draw(
                    painter,
                    center,
                    size,
                    getNorm() * (1 - DOUBLE_SIZE),
                    elements);
            }
        }
    }
};

/*
 * Symbol of an alphabet.
 *
 * It consists of elements: lines and curves.
 */
class Symbol {

    std::vector<Element> elements;

public:
    void add(Element element) {
        elements.push_back(element);
    }

    void draw(Painter* painter, Vector center, float size) {

        for (Element element : elements) {
            element.draw(painter, center, size, elements);
        }
    }
};

/*
 * Text representation of graphical element descriptor.
 *
 * E.g. `hto` means horizontal line placed on the top curved outwards.
 */
static std::unordered_map<char, ElementDescriptor> const descriptorMap
    = {{'h', ElementDescriptor::Horizontal},
       {'t', ElementDescriptor::Top},
       {'b', ElementDescriptor::Bottom},
       {'v', ElementDescriptor::Vertical},
       {'l', ElementDescriptor::Left},
       {'r', ElementDescriptor::Right},
       {'c', ElementDescriptor::Center},
       {'i', ElementDescriptor::CurvedInwards},
       {'o', ElementDescriptor::CurvedOutwards},
       {'I', ElementDescriptor::PointedInwards},
       {'O', ElementDescriptor::PointedOutwards},
       {'2', ElementDescriptor::Double},
       {'/', ElementDescriptor::Slash},
       {'\\', ElementDescriptor::Backslash},
       {'s', ElementDescriptor::Short},
       {'?', ElementDescriptor::Unknown}};

// Convert graphical element text representation into element descriptor.
ElementDescriptor getElementDescriptor(char elementRepr) {

    auto it = descriptorMap.find(elementRepr);

    if (it != descriptorMap.end()) {
        return it->second;
    } else {
        return ElementDescriptor::Unknown;
    }
}

// Split string by delimiter, ignore empty elements.
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (not token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::unordered_map<std::string, std::vector<std::string>>
parseGraphs(const std::string& path) {

    std::unordered_map<std::string, std::vector<std::string>> graphs;

    std::ifstream inFile(path);

    if (not inFile.is_open()) {
        throw std::invalid_argument("Could not open the file " + path + ".");
    }

    std::string line;

    while (std::getline(inFile, line)) {
        std::vector<std::string> row = split(line, ' ');
        std::string name = row[0];
        row.erase(row.begin());
        graphs[name] = row;
    }
    return graphs;
}

void drawTikz(
    Painter* painter,
    std::string ipaSymbol,
    std::vector<std::string> reprs,
    Vector center) {

    bool hasIpaSymbol = ipaSymbol != "-" and ipaSymbol != "=";
    bool isImpossible = ipaSymbol == "=";

    if (hasIpaSymbol) {
        painter->rectangle(
            center + Vector(-0.8, 0.25),
            center + Vector(0.2, -0.25),
            "draw=black");
        painter->text(center - Vector(0.8, 0), ipaSymbol, "anchor=west");
    } else if (isImpossible) {
        painter->rectangle(
            center + Vector(-0.8, 0.25),
            center + Vector(0.2, -0.25),
            "draw=black, fill=yellow!30");
        return;
    } else {
        painter->rectangle(
            center + Vector(-0.8, 0.25),
            center + Vector(0.2, -0.25),
            "draw=black, fill=yellow!15");
    }

    Symbol symbol = Symbol();

    for (int i = 0; i < reprs.size(); i++) {
        std::string repr = reprs[i];

        Element element = Element();

        for (char& elementRepr : repr) {
            ElementDescriptor elementDescriptor
                = getElementDescriptor(elementRepr);
            element.add(elementDescriptor);
        }
        symbol.add(element);
    }
    float size = 0.1f;
    symbol.draw(painter, center, size);
}

void parseConsonants(
    const std::string& path,
    std::unordered_map<std::string, std::vector<std::string>> graphs) {

    Painter* painter = new TikzPainter();

    std::ifstream inFile(path);

    if (not inFile.is_open()) {
        throw std::invalid_argument("Could not open the file " + path + ".");
    }

    std::string line;

    std::getline(inFile, line);
    std::vector<std::string> headers = split(line, ' ');

    float x = 0.0f;
    float y = 0.0f;

    for (int i = 0; i < headers.size(); i++) {
        std::string tex = std::regex_replace(headers[i], std::regex("_"), " ");
        tex = std::regex_replace(tex, std::regex(";"), ", ");
        painter->text(
            Vector(x - 0.5, y + 0.5),
            "{\\fontfamily{cmr}\\selectfont " + tex + "}",
            "anchor=west, rotate=30");
        x += 1;
    }
    x = 0.0f;

    while (std::getline(inFile, line)) {

        std::vector<std::string> row = split(line, ' ');
        std::vector<std::string> rowNames = split(row[0], ';');

        std::string tex = std::regex_replace(row[0], std::regex("_"), " ");
        tex = std::regex_replace(tex, std::regex(";"), ", ");
        painter->text(
            Vector(x - 6, y),
            "{\\fontfamily{cmr}\\selectfont " + tex + "}",
            "anchor=west");

        for (int i = 1; i < row.size(); i++) {

            std::string ipaSymbol = row[i];

            std::vector<std::string> columnNames = split(headers[i - 1], ';');

            std::vector<std::string> names = columnNames;
            names.insert(names.end(), rowNames.begin(), rowNames.end());

            std::vector<std::string> descriptors;

            for (int j = 0; j < names.size(); j++) {
                std::string name = names[j];
                if (graphs.contains(name)) {
                    for (int k = 0; k < graphs[name].size(); k++) {
                        std::string descriptor = graphs[name][k];
                        if (descriptor != ".") {
                            descriptors.push_back(graphs[name][k]);
                        }
                    }
                } else {
                    std::cerr << "Unknown name <" << name << ">" << std::endl;
                }
            }
            drawTikz(painter, ipaSymbol, descriptors, Vector(x, y));
            x += 1;
        }
        y -= 0.5;
        x = 0;
    }
    painter->end();

    std::cout << painter->getString();
}

int main(int argc, char** argv) {

    try {
        std::unordered_map<std::string, std::vector<std::string>> graphs
            = parseGraphs("data/graphs.txt");
        parseConsonants("data/consonants.txt", graphs);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
