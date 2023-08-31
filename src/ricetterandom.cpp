#include <clocale>
#include <csignal>
#include <curses.h>
#include <filesystem>
#include <fstream>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <random>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"
#include "../include/WindowManager.h"

namespace fs = std::filesystem;
namespace gz = giallozafferano;

// Path file contenente i path in cui si trovano le ricette scaricate dal programma Go.
const std::string kRecipesDirsPaths = "./recipesDirsPaths.txt";

// Comando per lanciare l'esecuzione del programama Go che scarica le ricette. (Compila ed esegue).
const std::string kDowloadRecipeCmd = "../RicetteRandom";

// Torna un numero random intero compreso nel range [min, max].
int random_int(int min, int max) {
    std::random_device rd;                           // random number from hardware
    std::mt19937 gen(rd());                          // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define range
    return distr(gen);
}

// Torna il nome del parent corrispondente al path passato.
std::string parent_name(const fs::path& path) {
    size_t pos = path.parent_path().string().find_last_of('/');
    return path.parent_path().string().substr(pos + 1);
}

// Torna una striga contenente il menu da stampare ad output.
std::string print_menu(const std::vector<std::string>& opts) {
    std::ostringstream menu;
    menu << "Scegli una delle seguenti portate:\n";
    int i = 1;
    for (const std::string& opt : opts)
        menu << "    "  << std::to_string(i++) + " - " << opt << '\n';
    menu << " Premi 'q' per uscire.";
    return menu.str();
}

// Torna un stringa formattata contenente le informazioni riguardianti la ricetta 
// passata.
std::string print_recipe_info(const gz::Recipe& recipe) {
    std::ostringstream oss;
    oss << "NOME: " << recipe.name() << '\n' << '\n'
        << "INGREDIENTI: " << recipe.ingredients() << '\n' << '\n'
        << "PREPARAZIONE: " << recipe.preparation();
    return oss.str();
}

int main(int argc, char **argv) {
    // gestione opt
    int parse;
    char* url = nullptr;
    while ((parse = getopt(argc, argv, "d:")) != -1) {
        switch (parse) {
            // lancia il dowload delle ricette
            case 'd':
                url = optarg;
                break;
            case '?':
                return 1;
        }
    }
    if (url) {
        std::system((kDowloadRecipeCmd + " " + url).c_str());
    } 

    // init book e deserializer
    gz::CookBook c_book{};
    gz::XmlRecipeDeserializer deserializer{};

    // deserializzo le ricette contenute in tutte le cartelle e le inserisco nel book
    std::ifstream ifile{kRecipesDirsPaths};
    for (std::string line; ifile >> line; ) {
        for (const fs::directory_entry& entry : fs::directory_iterator(line)) {
            c_book.insert(
                    parent_name(entry), 
                    deserializer.deserialize(entry.path().string())
            );
        }
    }

    // setup window manager
    std::shared_ptr<gz::WindowManager> wm = gz::WindowManager::get_instance();
    setlocale(LC_ALL, "");               // per caratteri unicode
    noecho();                            // non mostra input utente
    curs_set(0);                         // cursore non visibile
    signal(SIGWINCH, wm->handle_resize); // gestione ridimesionamento terminal

    // creazione finestre
    wm->create_win("info", 20, 40, 1, 142)->set_border();
    std::vector<std::string> types= c_book.get_recipes_types();
    wm->get_focused()->display_refresh(print_menu(types), 1, 1);
    wm->create_win("border-main", 41, 141, 0, 0)->set_border();
    wm->get_focused()->refresh();
    wm->create_win("main", 38, 138, 1, 1);

    // stampa random delle ricette
    char c{};
    while ((c = wm->get_focused()->get_ch()) != 'q') {
        int entry;
        if (isdigit(c) && (entry = int(c - '0')) <= types.size()) {
            wm->get_focused()->clear_content();
            std::vector<gz::Recipe> rec = c_book.get_recipes(types[entry - 1]);
            wm->get_focused()->display_refresh(print_recipe_info(rec[random_int(0, rec.size() - 1)]), 0, 0);
        } 
    }

    return 0;
}
