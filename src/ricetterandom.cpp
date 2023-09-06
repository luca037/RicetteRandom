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
    menu << " Premi 'f' per cercare\n";
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

// Stampa su win una ricetta random del tipo specificato.
void display_random_recipe(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book, const std::string& type) {
    std::vector<gz::Recipe> rec = book.get_recipes(type);
    if (rec.empty()) return;
    int rnd = random_int(0, rec.size() - 1);
    win->clear_content();
    win->display_refresh(print_recipe_info(rec[rnd]), 0, 0);
}

// Gestione ricerca ricetta e stampa risultati.
void display_find_recipe(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // setto il cursore visibile
    echo();
    curs_set(1);
    // ricerca ricetta
    win->clear_content();
    win->display_refresh(">> inserire nome: ", 0, 0);
    gz::Recipe found = book.find(win->get_str());
    if (found != gz::Recipe{}) {
        win->clear_content();
        win->display_refresh(print_recipe_info(found), 0, 0);
    } else {
        win->display_refresh(">> ricetta non trovata.", 1, 0);
    }
    // resetto impostazioni
    noecho();
    curs_set(0);
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
    // finestra menu
    wm->create_win("info", 20, 40, 1, 142)->set_border();
    wm->get_focused()->display_refresh(print_menu(c_book.get_recipes_types()), 1, 1);

    wm->create_win("border-main", 41, 141, 0, 0)->set_border();
    wm->get_focused()->refresh();
    wm->create_win("main", 38, 138, 1, 1);

    // stampa random delle ricette
    std::vector<std::string> types = c_book.get_recipes_types();
    char input{};
    while ((input = wm->get_focused()->get_ch()) != 'q') {
        int index = int(input - '0');
        if (isdigit(input) && index <= types.size() && index > 0) {
            display_random_recipe(wm->get_focused(), c_book, types[index - 1]);
        } else if (input =='f') {
            display_find_recipe(wm->get_focused(), c_book);
        }
    }

    return 0;
}
