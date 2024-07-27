#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "geometry.hpp"
#include "symbol.hpp"
#include "util.hpp"
#include "visual.hpp"

float DOUBLE_SIZE = 0.5;
float CURVE_SIZE = 0.5;
float CURVATURE = 0.6;

std::string sortParameters(std::string parameters) {

    std::vector<std::string> parametersVector = split(parameters, ';');
    std::sort(parametersVector.begin(), parametersVector.end());

    std::ostringstream result;

    for (std::string parameter : parametersVector) {
        result << parameter << ';';
    }
    return result.str();
}

Vector Element::getNorm() {
    return indirectedNorm * position;
}

Vector Element::getPoint1() {
    return getNorm() + direction;
}

Vector Element::getPoint2() {
    return getNorm() - direction;
}

void Element::add(ElementDescriptor elementDescriptor) {

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
    default:
        break;
    }
}

void Element::draw(
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
                } else if (element.getNorm().isGridCodirectedTo(direction)) {
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
                if ((step.x == 1 or step.x == -1 or step.y == 1 or step.y == -1)
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
void Element::draw(
    Painter* painter,
    Vector center,
    float size,
    std::vector<Element> elements) {

    if (this->isDouble and position == 0) {
        draw(painter, center, size, getNorm() * 0.3f, elements);
        draw(painter, center, size, getNorm() * -1 * 0.3f, elements);
    } else {
        draw(painter, center, size, getNorm() * 1.0f, elements);
        if (this->isDouble) {
            draw(
                painter, center, size, getNorm() * (1 - DOUBLE_SIZE), elements);
        }
    }
}

void Symbol::add(Element element) {
    elements.push_back(element);
}

void Symbol::draw(Painter* painter, Vector center, float size) {

    for (Element element : elements) {
        element.draw(painter, center, size, elements);
    }
}

// Convert graphical element text representation into element descriptor.
ElementDescriptor getElementDescriptor(char elementRepr) {

    auto it = descriptorMap.find(elementRepr);

    if (it != descriptorMap.end()) {
        return it->second;
    } else {
        return ElementDescriptor::Unknown;
    }
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

    bool hasIpaSymbol
        = ipaSymbol != "-" and ipaSymbol != "=" and ipaSymbol != " ";
    bool isImpossible = ipaSymbol == "=";

    if (hasIpaSymbol) {
        painter->text(center - Vector(0, 0), "\\doulos{" + ipaSymbol + "}", "");
    } else {
        if (isImpossible) { }
        return;
    }

    Symbol symbol = Symbol();

    for (unsigned i = 0; i < reprs.size(); i++) {
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
    symbol.draw(painter, center + Vector(0.5, 0), size);
}

void IpaSymbols::add(std::string parameters, std::string ipaSymbol) {
    std::string key = sortParameters(parameters);
    symbols[key] = ipaSymbol;
}

std::string IpaSymbols::findSymbol(std::string key) {
    if (symbols.contains(key)) {
        return symbols[key];
    }
    return " ";
}

std::string parametersToTex(std::string parameters) {
    parameters = std::regex_replace(parameters, std::regex("_"), " ");
    parameters = std::regex_replace(parameters, std::regex(";"), ", ");
    return parameters;
}

void drawTable(
    Painter* painter,
    std::vector<std::string> columns,
    std::vector<std::string> rows,
    IpaSymbols* ipaSymbols,
    std::unordered_map<std::string, std::vector<std::string>> graphs) {

    float x = 0.0f;
    float y = 0.0f;

    float xStep = 1.0f;
    float yStep = 0.5f;

    for (unsigned i = 0; i <= columns.size(); i++) {
        painter->line(
            Vector(i * xStep, 0),
            Vector(i * xStep, rows.size() * -yStep),
            "draw=black");
        if (i < columns.size()) {
            painter->text(
                Vector(i * xStep + 0.5, y + 0.3),
                parametersToTex(columns[i]),
                "anchor=west, rotate=30");
        }
    }
    for (unsigned i = 0; i <= rows.size(); i++) {
        painter->line(
            Vector(0, i * -yStep),
            Vector(columns.size() * xStep, i * -yStep),
            "draw=black");
        if (i < rows.size()) {
            painter->text(
                Vector(x - 0.1, i * -yStep - (yStep / 2)),
                parametersToTex(rows[i]),
                "anchor=east");
        }
    }

    for (std::string row : rows) {
        for (std::string column : columns) {

            std::string parameters = column + ";" + row;
            std::vector<std::string> parametersVector = split(parameters, ';');
            std::string key = sortParameters(parameters);
            std::string ipaSymbol = ipaSymbols->findSymbol(key);

            std::vector<std::string> descriptors;

            for (std::string parameter : parametersVector) {
                if (graphs.contains(parameter)) {
                    for (unsigned k = 0; k < graphs[parameter].size(); k++) {
                        std::string descriptor = graphs[parameter][k];
                        if (descriptor != ".") {
                            descriptors.push_back(graphs[parameter][k]);
                        }
                    }
                } else {
                    std::cerr << "Unknown parameter <" << parameter << ">"
                              << std::endl;
                }
            }
            drawTikz(
                painter, ipaSymbol, descriptors, Vector(x + 0.25, y - 0.25));
            x += xStep;
        }
        y -= yStep;
        x = 0;
    }
    painter->end();
    std::cout << painter->getString();
}
