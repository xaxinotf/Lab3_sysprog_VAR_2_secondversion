#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include <windows.h>

#define FOREGROUND_CYAN 0
#define FOREGROUND_MAGENTA 0
#define FOREGROUND_WHITE 0
// token classes
std::map<std::string, std::regex> regexes = {
        {"preprocessor", std::regex("#include[ \t]*<.*?>")},
        {"number", std::regex("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?")},
        {"hexadecimal", std::regex("0x[0-9a-fA-F]+")},
        {"string_constant", std::regex("\".*?\"")},
        {"char_constant", std::regex("'.'")},
        {"comment", std::regex("//.*?\\n")},
        {"reserved_words", std::regex("\\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\\b")},
        {"operators", std::regex("[+=\\-*/%&|!^<>=~]")},
        {"punctuation", std::regex("[(){},;]")},
        {"identifier", std::regex("[a-zA-Z_][a-zA-Z0-9_]*")}
};
std::map<std::string, WORD> colors = {
        {"preprocessor", FOREGROUND_BLUE | FOREGROUND_INTENSITY},
        {"number", FOREGROUND_GREEN},
        {"hexadecimal", FOREGROUND_GREEN | FOREGROUND_INTENSITY},
        {"string_constant", FOREGROUND_RED},
        {"char_constant", FOREGROUND_RED | FOREGROUND_INTENSITY},
        {"comment", FOREGROUND_BLUE},
        {"reserved_words", FOREGROUND_CYAN},
        {"operators", FOREGROUND_MAGENTA},
        {"punctuation", FOREGROUND_CYAN | FOREGROUND_INTENSITY},
        {"identifier", FOREGROUND_WHITE},
        {"error", BACKGROUND_RED | FOREGROUND_WHITE | FOREGROUND_INTENSITY}
};

std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    return content;
}

int main() {
    std::string input = read_file("input.txt");
    std::ofstream out("out.txt");

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    for (auto &pair : regexes) {
        std::smatch match;
        std::string::const_iterator start = input.cbegin(), end = input.cend();

        while (std::regex_search(start, end, match, pair.second)) {
            std::string token = match[0];
            out << "<" << token << ", " << pair.first << ">" << std::endl;
            SetConsoleTextAttribute(handle, colors[pair.first]);
            std::cout << token;
            SetConsoleTextAttribute(handle, FOREGROUND_WHITE); // Reset color
            std::cout << " ";

            start = match.suffix().first;
        }
    }

    // errors (unrecognized characters)
    std::regex error_regex("[^a-zA-Z0-9_+=\\-*/%&|!^<>=~#\"'().,{};\\s]");
    std::smatch match;
    std::string::const_iterator start = input.cbegin(), end = input.cend();

    while (std::regex_search(start, end, match, error_regex)) {
        std::string token = match[0];
        out << "<" << token << ", error>" << std::endl;
        start = match.suffix().first;
    }

    out.close();
    return 0;
}
