#include "../include/Recipe.h"

#include <algorithm>
#include <cctype>

namespace giallozafferano {

std::string Recipe::tolower(const std::string& s) {
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(),
            [](unsigned char c) { return std::tolower(c); });
    return tmp;
}

}

