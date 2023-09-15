#include "../include/Recipe.h"

#include <algorithm>
#include <cctype>

namespace giallozafferano {

std::string tolower(const std::string& s) {
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(),
            [](unsigned char c) { return std::tolower(c); });
    return tmp;
}

bool operator==(const Recipe& lhs, const Recipe& rhs) { 
    return tolower(lhs.name()) == tolower(rhs.name());
}

bool operator!=(const Recipe& lhs, const Recipe& rhs) { 
    return tolower(lhs.name()) != tolower(rhs.name());
}

std::ostream& operator<<(std::ostream& os, const Recipe& r) {
    return os << "{" 
        << std::quoted("Name") << ": " << std::quoted(r.name()) << ", "
        << std::quoted("Ingredients") << ": " << std::quoted(r.ingredients()) << ", "
        << std::quoted("Preparation") << ": " << std::quoted(r.preparation())
        << "}";
}

}

