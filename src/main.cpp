#include <fstream>
#include <iostream>
#include <unordered_map>

#include "geometry.hpp"
#include "symbol.hpp"
#include "util.hpp"
#include "visual.hpp"

IpaSymbols* parseTables(const std::string& path) {

    IpaSymbols* ipaSymbols = new IpaSymbols();

    std::ifstream inFile(path);

    std::string line;
    std::getline(inFile, line);
    std::vector<std::string> columns = split(line, ' ');

    while (std::getline(inFile, line)) {

        if (line.empty()) {
            std::getline(inFile, line);
            columns = split(line, ' ');
            continue;
        }
        std::vector<std::string> parts = split(line, ' ');
        std::string row = parts[0];

        for (unsigned i = 1; i < parts.size(); i++) {

            std::string parameters = columns[i - 1] + ';' + row;
            ipaSymbols->add(parameters, parts[i]);
        }
    }
    return ipaSymbols;
}

void drawTable(
    std::vector<std::string> rows, std::vector<std::string> columns) {
    std::unordered_map<std::string, std::vector<std::string>> graphs
        = parseGraphs("data/graphs.txt");
    IpaSymbols* ipaSymbols = parseTables("data/consonants.txt");
    drawTable(
        new TikzPainter("out/ipa_table.tex"),
        rows,
        columns,
        ipaSymbols,
        graphs);
}

void drawSymbol(std::vector<std::string> parameters) {
    Painter* painter = new TikzPainter("out/ipa_table.tex");
    Symbol symbol = Symbol(parameters);
    symbol.draw(painter, Vector(0, 0), 0.1f);
    painter->end();
    std::cout << painter->getString();
}

int main(int argc, char** argv) {

    try {
        if (std::string(argv[1]) == "table") {
            if (argc != 4) {
                std::cerr << "`table` command should have exactly two "
                             "arguments: rows and columns."
                          << std::endl;
                return 1;
            }
            std::vector<std::string> rows = split(argv[2], ',');
            std::vector<std::string> columns = split(argv[3], ',');

            drawTable(rows, columns);

        } else if (std::string(argv[1]) == "symbol") {
            std::vector<std::string> parameters;
            for (int i = 0; i < argc - 2; i++) {
                parameters.push_back(std::string(argv[i + 2]));
            }
            drawSymbol(parameters);

        } else {
            std::cerr << "First argument should be `table` or `symbol`."
                      << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
