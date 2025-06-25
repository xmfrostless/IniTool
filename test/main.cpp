#include <IniTool.h>
#include <iostream>
#include <fstream>
#include <string>

std::string Load(const std::string& filename) {
    std::ifstream in(filename, std::ios::ate);
    if (!in.is_open()) {
        return "";
    }
    auto size { in.tellg() };
    if (size == 0) {
        return "";
    }
    in.seekg(0);
    std::string buffer;
    buffer.resize(static_cast<std::size_t>(size));
    in.read((char*)buffer.data(), size);
    return buffer;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        auto text = Load(argv[1]);
        auto obj = INI::Parse(text);
        std::cout << INI::Stringify(obj) << std::endl;
    } else {
        auto text = Load("./test.ini");
        auto obj = INI::Parse(text);
        std::cout << INI::Stringify(obj) << std::endl;
    }

    return 0;
}