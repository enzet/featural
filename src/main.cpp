#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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

int main(int argc, char** argv) {

    try {
        std::unordered_map<std::string, std::vector<std::string>> graphs
            = parseGraphs("data/graphs.txt");
        IpaSymbols* ipaSymbols = parseTables("data/consonants.txt");
        drawTable(
            new TikzPainter("out/ipa_table.tex"),
            {"labial",
             "labial;dental",
             "dental",
             "alveolar",
             "postalveolar",
             "retroflex",
             "palatal",
             "velar",
             "uvular",
             "pharyngeal",
             "glottal"},
            {"plosive;voiceless",
             "plosive;voiced",
             "nasal;voiceless",
             "nasal;voiced",
             "trill;voiceless",
             "trill;voiced",
             "tap/flap;voiceless",
             "tap/flap;voiced",
             "sibilant_fricative;voiceless",
             "sibilant_fricative;voiced",
             "non-sibilant_fricative;voiceless",
             "non-sibilant_fricative;voiced",
             "lateral_fricative;voiceless",
             "lateral_fricative;voiced",
             "approximant;voiced",
             "lateral_approximant;voiceless",
             "lateral_approximant;voiced"},
            ipaSymbols,
            graphs);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
