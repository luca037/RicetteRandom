#include <iostream>
#include <filesystem>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"

namespace fs = std::filesystem;
namespace gz = giallozafferano;

const std::string kRecipesDirPath = "../ricette/";

int main() {
    gz::CookBook c_book{};
    gz::XmlRecipeDeserializer deserializer{};
    for (const fs::directory_entry& entry : fs::directory_iterator(kRecipesDirPath)) {
        c_book.insert(deserializer.deserialize(entry.path().c_str()));
    }
    std::cout << c_book << std::endl;
    return 0;
}
