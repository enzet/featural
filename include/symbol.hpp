#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>

#include "visual.hpp"

/* Describes a graphical element: how and where to draw it. */
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

/* Sort parameters sequence separated by `;`. */
std::string sortParameters(std::string parameters);

/* Style of a symbol. */
class SymbolStyle {

public:
    float lineWidth = 0.4f;
    Vector position = Vector(0, 0);
    bool useBackground = false;
    float zoom = 1.0f;
    bool shiftByCurved = true;
    bool curveDiagonal = true;
    bool isHandwritten = false;

    SymbolStyle(std::vector<std::string> description);
};

/*
 * Symbol element.
 *
 * Symbol element is a line, a curved line or a double line, that is drawn in a
 * square limited by [-1, 1] on X axis and by [-1, 1] on Y axis.
 *
 *  point 1   point 2
 *     *----*----*
 *          ↑
 *     norm |
 *          *---->
 *           direction
 *
 * Offsets:
 *
 *     *----*       [-1, 0]
 *
 *     *----*----*  [-1, 1]
 *
 *          *----*  [0, 1]
 */
class Element {

    std::vector<ElementDescriptor> elementDescriptors;
    Vector indirectedNorm = Vector();
    float position = 0;
    Vector direction = Vector();
    float pointOffset1 = -1;
    float pointOffset2 = 1;

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
        SymbolStyle style,
        Vector center,
        float size,
        Vector step,
        std::vector<Element> elements);

    /* Draw symbol element. */
    void draw(
        Painter* painter,
        SymbolStyle style,
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
    void add(Element element);

public:
    /* Construct symbol from the string representation. */
    Symbol(std::vector<std::string> reprs);

    /* Get graphical representation of the symbol. */
    void draw(Painter* painter, SymbolStyle style, Vector center, float size);
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

/* Convert graphical element text representation into element descriptor. */
ElementDescriptor getElementDescriptor(char elementRepr);

std::pair<Symbol, SymbolStyle>
parseSymbolParameters(std::vector<std::string> parameters);

/*
 * Parse graphs file.
 *
 * The file has the following structure: it has space-separated data on each
 * line, where the first element is a parameter of a phoneme and the rest are
 * descriptors of graphical elements.
 *
 * E.g. `sibilant_affricate ht hbo`, which means, that if sound is a sibilant
 * affricate, its symbol should have horizontal top line and horizontal bottom
 * line curved outwards.
 */
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

/*
 * Draw phonetic table.
 *
 * Rows ans columns contain phonological characteristics.
 */
void drawTable(
    Painter* painter,
    std::vector<std::string> columns,
    std::vector<std::string> rows,
    std::vector<std::string> filter,
    IpaSymbols* ipaSymbols,
    std::unordered_map<std::string, std::vector<std::string>> graphs);

#endif
