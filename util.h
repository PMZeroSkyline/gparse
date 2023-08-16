#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace util {

bool fileExists(const fs::path &filePath) {
    return fs::exists(filePath) && fs::is_regular_file(filePath);
}
fs::path getDirectoryPath(const fs::path &filePath) {
    return filePath.parent_path();
}
bool createDirectoryIfNotExists(const fs::path &path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        return true; // Directory already exists
    }

    try {
        fs::create_directories(path);
        return true;
    } catch (const fs::filesystem_error &ex) {
        std::cerr << "Error creating directory: " << ex.what() << std::endl;
        return false;
    }
}
bool containsSubstring(const std::string &str, const std::string &subStr, bool ignoreCase = false) {
    if (ignoreCase) {
        std::string strLower = str;
        std::string subStrLower = subStr;
        std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
        std::transform(subStrLower.begin(), subStrLower.end(), subStrLower.begin(), ::tolower);
        return strLower.find(subStrLower) != std::string::npos;
    } else {
        return str.find(subStr) != std::string::npos;
    }
}
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}
std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::string token;
    for (char c : input) {
        if (c == delimiter) {
            result.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        result.push_back(token);
    }
    return result;
}
std::string removeSpaces(const std::string& input) {
    std::string result;
    
    for (char c : input) {
        if (c != ' ') {
            result += c;
        }
    }
    return result;
}
int findItemIndex(const std::vector<std::string>& vec, const std::string& target) {
    auto it = std::find(vec.begin(), vec.end(), target);
    if (it != vec.end()) {
        return std::distance(vec.begin(), it); // Calculate the index using iterator distance
    }
    return -1; // Return -1 if the target is not found in the vector
}
std::string removeParentheses(const std::string& input) {
    std::string result;
    
    for (char c : input) {
        if (c != '(' && c != ')') {
            result += c;
        }
    }
    
    return result;
}
std::string getFileExtension(const std::string& filePath) {
    size_t dotPosition = filePath.find_last_of(".");
    if (dotPosition == std::string::npos) {
        return "";
    }

    return filePath.substr(dotPosition + 1);
}

}