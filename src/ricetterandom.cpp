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

// Opzioni applicazione.
const std::string kMenu{
    "Scegli una delle seguenti opzioni:\n \
    1 - Ricetta random; \n \
    2 - Ricerca ricetta;\n \
    q - Per uscire."
};

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

// Torna una striga contenente il menu delle portate.
// La numerazione delle opzioni parte dall'indice 1.
std::string print_types_menu(const std::vector<std::string>& opts) {
    std::ostringstream menu;
    menu << "Scegli una delle seguenti portate:\n";
    int i = 1;
    for (const std::string& opt : opts)
        menu << "    "  << std::to_string(i++) + " - " << opt << '\n';
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
void random_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // stampa menu di selezione portata
    std::vector<std::string> types_list = book.get_recipes_types();
    win->clear_content();
    win->display_refresh(print_types_menu(types_list), 0, 0);
    int selected = int(win->get_ch() - '0');
    if (selected > types_list.size() || selected <= 0) 
        return;
    // stampa ricetta random tra quelle
    std::vector<gz::Recipe> rec = book.get_recipes(types_list[selected - 1]);
    if (rec.empty())
        return; 
    win->clear_content();
    win->display_refresh(print_recipe_info(rec[random_int(0, rec.size() - 1)]), 0, 0);
}

// Gestione ricerca ricetta e stampa risultati.
void find_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
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
    wm->create_win("menu", 20, 40, 0, 142)->set_border();
    wm->get_focused()->display_refresh(kMenu, 1, 1);

    // finestra principale
    wm->create_win("border-main", 41, 141, 0, 0)->set_border(); // funge solo da bordo
    wm->get_focused()->refresh();
    wm->create_win("main", 38, 138, 1, 1);

    // stampa random delle ricette
    char input{};
    while ((input = wm->get_focused()->get_ch()) != 'q') {
        if (input == '1') {
            random_recipe_opt(wm->get_focused(), c_book);
        } else if (input =='2') {
            find_recipe_opt(wm->get_focused(), c_book);
        }
    }

    return 0;
}
