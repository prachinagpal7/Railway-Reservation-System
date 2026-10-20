#ifndef CONFIG_H
#define CONFIG_H
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>

inline std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    if(start == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

inline void loadEnvFile(const std::string& filename = ".env") {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if(line.empty() || line[0] == '#') continue;
        const auto pos = line.find('=');
        if(pos == std::string::npos) continue;
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        if(!key.empty())  _putenv_s(key.c_str(), value.c_str());
    }
}
#endif