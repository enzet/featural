#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <vector>
#include <string>

#include "visual.hpp"

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
 * Sort parameters sequence separated by `;`.
 */
std::string sortParameters(std::string parameters);

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

    bool isDouble = false;
    bool isCurved = false;
    bool isPointed = false;
    bool isInwards = false;
    bool isDiagonal = false;

    Vector getNorm();
    Vector getPoint1();
    Vector getPoint2();

public:
    void add(ElementDescriptor elementDescriptor);
    void draw(
        Painter* painter,
        Vector center,
        float size,
        Vector step,
        std::vector<Element> elements);

    /*
     * Draw symbol element.
     */
    void draw(
        Painter* painter,
        Vector center,
        float size,
        std::vector<Element> elements);
};

/*
 * Symbol of an alphabet.
 *
 * It consists of elements: lines and curves.
 */
class Symbol {

    std::vector<Element> elements;

public:
    void add(Element element);
    void draw(Painter* painter, Vector center, float size);
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

/*
 * Convert graphical element text representation into element descriptor.
 */
ElementDescriptor getElementDescriptor(char elementRepr);

std::unordered_map<std::string, std::vector<std::string>>
parseGraphs(const std::string& path);

void drawTikz(
    Painter* painter,
    std::string ipaSymbol,
    std::vector<std::string> reprs,
    Vector center);

class IpaSymbols {

    std::unordered_map<std::string, std::string> symbols;

public:
    void add(std::string parameters, std::string ipaSymbol);
    std::string findSymbol(std::string key);
};

std::string parametersToTex(std::string parameters);

void drawTable(
    Painter* painter,
    std::vector<std::string> columns,
    std::vector<std::string> rows,
    IpaSymbols* ipaSymbols,
    std::unordered_map<std::string, std::vector<std::string>> graphs);

#endif
