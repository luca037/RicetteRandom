#include <iostream>
#include <filesystem>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <random>
#include <fstream>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"
#include "../include/Window.h"

namespace fs = std::filesystem;
namespace gz = giallozafferano;

const std::string kRecipesDirsPaths = "../recipesDirsPaths.txt";
constexpr char kDowloadRecipeCmd[] = "go run ../dowloadRecipes.go";

int random_int(int min, int max) {
    std::random_device rd; // random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max);  // define range
    return distr(gen);
}

int main(int argc, char **argv) {

    // manage command line opt
    int parse;
    while ((parse = getopt(argc, argv, "d")) != -1) {
        switch (parse) {
            case 'd':
                std::system(kDowloadRecipeCmd);
        }
    }

    // init
    gz::initenv();
    gz::Window win{80, 80, 0, 0};
    gz::CookBook c_book{};
    gz::XmlRecipeDeserializer deserializer{};

    // insert recipes
    std::ifstream ifile{kRecipesDirsPaths};
    for (std::string line; ifile >> line; ) {
        for (const fs::directory_entry& entry : fs::directory_iterator(line)) {
            c_book.insert(deserializer.deserialize(entry.path().string()));
        }
    }

    // generate random recipes
    win.display("Type 'n' to generate a random recipe ot 's' to quit.", 0, 0);
    char c{};
    while ((c = win.get_ch()) != 's') {
        win.clear();
        if (c == 'n') {
            win.display(c_book.get_recipes()[random_int(0, c_book.size() - 1)].name().c_str(), 0, 0);
            win.refresh();
        }
    }
    
    gz::endenv();

    return 0;
}
