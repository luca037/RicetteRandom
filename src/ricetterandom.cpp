#include <iostream>
#include <filesystem>
#include <fstream>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <random>
#include <memory>
#include <string>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"
#include "../include/WindowManager.h"

namespace fs = std::filesystem;
namespace gz = giallozafferano;

const std::string kRecipesDirsPaths = "./recipesDirsPaths.txt";
constexpr char kDowloadRecipeCmd[] = "../RicetteRandom";

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
    std::shared_ptr<gz::WindowManager> wm = gz::WindowManager::get_instance();
    wm->create_win("bg-border", 41, 81)->set_border();
    wm->get_focused()->refresh();
    wm->create_win("main", 39, 79, 1, 1);
    wm->get_focused()->display_refresh("Type 'n' to generate a random recipe ot 's' to quit.", 0, 0);

    //// piccolo test
    //wm->create_win("input", 20, 40, 20, 20)->set_border();
    //wm->get_focused()->display("Premere quls per uscire", 1, 1);
    //wm->get_focused()->refresh();

    //bool found;
    //if ((found = wm->find_win("main")))
    //    wm->get_focused()->display(std::to_string(found).c_str(), 0, 0);
    //wm->get_focused()->refresh();

    //wm->get_focused()->get_ch();

    char c{};
    while ((c = wm->get_focused()->get_ch()) != 's') {
        wm->get_focused()->clear();
        if (c == 'n') {
            wm->get_focused()->display_refresh(c_book.get_recipes()[random_int(0, c_book.size() - 1)].name().c_str(), 0, 0);
        } else {
            wm->get_focused()->display_refresh("Type 'n' to generate a random recipe or 's' to quit.", 0, 0);
        }
    }

    return 0;
}
