#pragma once

#include <string>
#include <sstream>
#include <string_view>
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <locale>
#include <vector>

//For both Windows and Linux systems
bool isValidFilename(const std::string& name) {
    if (name.empty())
        return false;

    // Cannot be "." or ".."
    if (name == "." || name == "..")
        return false;

    // Windows forbidden characters
    const std::string forbidden = "<>:\"/\\|?*";

    for (unsigned char c : name) {
        // Control characters (0–31)
        if (c < 32)
            return false;

        if (forbidden.find(c) != std::string::npos)
            return false;
    }

    // Cannot end with space or dot (Windows restriction)
    if (name.back() == ' ' || name.back() == '.')
        return false;

    // Windows reserved device names (case-insensitive)
    static const std::unordered_set<std::string> reserved = {
        "CON", "PRN", "AUX", "NUL",
        "COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9",
        "LPT1","LPT2","LPT3","LPT4","LPT5","LPT6","LPT7","LPT8","LPT9"
    };

    // Extract base name before dot (e.g. "CON.txt" is still invalid)
    std::string base = name;
    auto dotPos = name.find('.');
    if (dotPos != std::string::npos)
        base = name.substr(0, dotPos);

    // Convert to uppercase for comparison
    std::string upperBase = base;
    std::transform(upperBase.begin(), upperBase.end(), upperBase.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    if (reserved.find(upperBase) != reserved.end())
        return false;

    return true;
}


std::string& toLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c){ return std::tolower(c); });
    return str;
}

bool lowercaseComparison(std::string a, std::string b) {
    std::string a_ = a;
    std::string b_ = b;
    toLower(a_); toLower(b_);

    return a_ == b_;
}

// Map support
struct CaseInsensitiveHash {
    size_t operator()(const std::string& s) const {
        std::string temp = s;
        toLower(temp); 
        return std::hash<std::string>{}(temp);
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& a, const std::string& b) const {
        return lowercaseComparison(a, b);
    }
};


// Trim from the start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from the end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

//Convenience function
inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}



void replace_char_with_strings(std::string &s, char pattern, std::string_view replacement) {
    auto it = s.begin();
    while ((it = std::find(it, s.end(), pattern)) != s.end()) {
        it = s.erase(it);
        it = s.insert(it, replacement.begin(), replacement.end());
    }
}

template <typename T>
std::string serializeToDashed(const std::vector<T>& vec) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        
        if (i < vec.size() - 1) {
            oss << "-";
        }
    }

    return oss.str();
}

std::vector<int> unserializeDashedIntegers(const std::string& serialized) {
    std::vector<int> unserialized;
    int i = 0;
    int i_prev = i;
    while (i < serialized.size()) {
        i = serialized.find('-', i);
        if (i==std::string::npos) i = serialized.size();
        unserialized.push_back(std::stoi(serialized.substr(i_prev, i-i_prev)));
        i_prev = ++i;
    }

    return unserialized;
}