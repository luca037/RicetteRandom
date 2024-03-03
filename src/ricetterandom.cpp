#include <clocale>
#include <csignal>
#include <curses.h>
#include <filesystem>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"
#include "../include/WindowManager.h"
#include "./ricetterandom_utils.cpp"

namespace fs = std::filesystem;
namespace gz = giallozafferano;

// Path cartella che contiene tutte le cartelle contenenti le ricette serializzate
const std::string kRecipesDirDefaultPath = "./ricettario";

// Comando per lanciare l'esecuzione del programama Go che scarica le ricette.
const std::string kDowloadRecipeCmd = "../RicetteRandom";

// Opzioni applicazione.
const std::vector<std::string> kMenuOpt{
    "Ricetta random",
    "Ricerca ricetta",
    "Naviga ricettario"
};

const std::string kVimKeys{
    "Comandi:\n \
    j -> Down;\n \
    k -> Up;\n \
    h -> Left;\n \
    l -> Right."
};

// Gestione opzione ricetta random.
void random_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // salvo le portate (non cambia durante l'esecuzione)
    static const std::vector<std::string> types{book.get_recipes_types()};

    // stampa menu di selezione portata
    win->clear_content();
    win->attribute_on({A_BOLD, A_REVERSE});
    win->display("- Random - Scegli una delle seguenti portate:", 0, 0);
    win->attribute_off({A_BOLD, A_REVERSE});

    // stampa ricetta random
    std::string selected{};
    while ((selected = manage_cursor(win, 1, 2, types)) != "") {
        // salvo le ricette della portata selezionata
        std::map<std::string, gz::Recipe> recipes{book.get_recipes(selected)};

        // stampo una ricetta random finché l'untente non torna al menù prec
        char input{'j'};
        do {
            if (input == 'j') {
                win->clear_content();
                auto it = recipes.begin();
                std::advance(it, random_int(0, recipes.size() - 1));
                display_recipe_info(win, it->second);
            }
        } while ((input = win->get_ch()) != 'h');

        //ristampo il menu di selezione portata (per quando torno indietro)
        win->clear_content();
        win->attribute_on({A_BOLD, A_REVERSE});
        win->display("- Random - Scegli una delle seguenti portate:", 0, 0);
        win->attribute_off({A_BOLD, A_REVERSE});
    }
    win->clear_content();
}

// Gestione menu di navigazione.
void navigate_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // salvo le portate (non cambia durante l'esecuzione)
    static const std::vector<std::string> types{book.get_recipes_types()};

    // display menu iniziale
    win->clear_content();
    win->attribute_on({A_BOLD, A_REVERSE});
    win->display("- Navigazione -", 0, 0);
    win->attribute_off({A_BOLD, A_REVERSE});

    // gestione navigazione
    std::string selected{};
    while ((selected = manage_cursor(win, 1, 2, types)) != "") {
        // salvo le ricette della portata selezionata
        std::map<std::string, gz::Recipe> recipes{book.get_recipes(selected)};

        // stampa ricette 
        win->clear_content();
        auto it = recipes.begin();
        display_recipe_info(win, it->second);
        int current_index{1}; // indice ricetta corrente
        win->display_refresh("[" + std::to_string(current_index) + "/" + std::to_string(recipes.size()) + "]", win->height() - 1, 0);
        for (char c{}; c != 'h'; c = win->get_ch()) {
            switch (c) {
            case 'j':
                if (++it != recipes.end()) {
                    win->clear_content();
                    display_recipe_info(win, it->second);
                    ++current_index;
                    win->display_refresh("[" + std::to_string(current_index) + "/" + std::to_string(recipes.size()) + "]", win->height() - 1, 0);
                } else {
                    --it; // per farlo puntare sempre al penultimo elemento della mappa
                }
                break;
            case 'k':
                if (it != recipes.begin()) {
                    --it;
                    win->clear_content();
                    display_recipe_info(win, it->second);
                    --current_index;
                    win->display_refresh("[" + std::to_string(current_index) + "/" + std::to_string(recipes.size()) + "]", win->height() - 1, 0);
                }
                break;
            }
        }

        // display menu iniziale (se torno indietro dalla selezione)
        win->clear_content();
        win->attribute_on({A_BOLD, A_REVERSE});
        win->display("- Navigazione -", 0, 0);
        win->attribute_off({A_BOLD, A_REVERSE});
    }
    win->clear_content();
}

// Gestione ricerca ricetta e stampa risultati.
void find_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // inizializzo ambiente
    echo();
    curs_set(1);
    win->clear_content();
    win->display_refresh(">> inserire nome: ", 0, 0);

    // ricerca ricetta
    int y{}; // ordinata corrente
    std::string input;
    while ((input = win->get_str()) != "q") {
        gz::Recipe found;
        if (( found = book.find(input)) != gz::Recipe{}) {
            win->clear_content();
            noecho();
            display_recipe_info(win, found);

            // aspetto prima di tornare alla ricerca
            for (char c{}; c != 'h'; c = win->get_ch()) /* wait */;
            win->clear_content();
            win->display_refresh(">> inserire nome: ", 0, 0);
            y = 0;
            echo();
        } else {
            // controllo se ho raggiunto il limite della finestra
            if (y + 2 > win->height()) { win->clear_content(); y = -1; }
            win->display("error:  ricetta non trovata.", ++y, 0);
            win->display_refresh(">> inserire nome: ", ++y, 0);
        }
    }

    // resetto impostazioni di partenza
    noecho();
    curs_set(0);
    win->clear_content();
}

int main(int argc, char **argv) {
    // gestione opt
    int parse;
    char* url{nullptr};
    bool quit{};
    while ((parse = getopt(argc, argv, "qd:")) != -1) {
        switch (parse) {
            // lancia il dowload delle ricette
            case 'd':
                url = optarg;
                break;
            // esce dopo il download
            case 'q':
                quit = true;
                break;
            case '?':
                return 1;
        }
    }
    if (url) {
        std::system((kDowloadRecipeCmd + " " + url).c_str());
        if (quit) return 0;
    } 

    // init book e deserializer
    gz::CookBook c_book{};
    gz::XmlRecipeDeserializer deserializer{};

    // deserializzo tutte le ricette contenute nelle cartelle contenute
    // nella cartella kRecipesDirPath
    if (fs::exists(kRecipesDirDefaultPath) && fs::is_directory(kRecipesDirDefaultPath)) {
        for (const fs::directory_entry& recipes_type : fs::directory_iterator(kRecipesDirDefaultPath)) {
            if (fs::is_directory(recipes_type)) {
                for (const fs::directory_entry& recipe : fs::directory_iterator(recipes_type)) {
                    if (recipe.is_regular_file()) {
                        c_book.insert(
                                recipes_type.path().filename(), 
                                deserializer.deserialize(recipe.path().string())
                                );
                    }
                }
            }
        }
    } else { // se non è stata trovato il ricettario
        std::cerr << "Directory \'" << kRecipesDirDefaultPath << "\' was not found.\n"
                  << "Use the -d <url> option to download some recipes.";
        return 1;
    }
 
    // setup window manager
    std::shared_ptr<gz::WindowManager> wm = gz::WindowManager::get_instance();
    setlocale(LC_ALL, "");               // per caratteri unicode
    noecho();                            // non mostra input utente
    curs_set(0);                         // cursore non visibile
    signal(SIGWINCH, wm->handle_resize); // gestione ridimesionamento terminal

    // info box keybindings
    wm->create_win("vim keys", 8, 40, 0, 152)->set_border();
    wm->get_focused()->display_refresh(kVimKeys, 1, 1);

    // finestra principale
    wm->create_win("main", 46, 148, 0, 0);

    // gestione comandi
    std::string input{};
    do {
        if (input == kMenuOpt[0]) {
            random_recipe_opt(wm->get_focused(), c_book);
        } else if (input == kMenuOpt[1]) {
            find_recipe_opt(wm->get_focused(), c_book);
        } else if (input == kMenuOpt[2]) {
            navigate_opt(wm->get_focused(), c_book);
        }
        wm->get_focused()->attribute_on({A_BOLD, A_REVERSE});
        wm->get_focused()->display("- Menu principale -", 0, 0);
        wm->get_focused()->attribute_off({A_BOLD, A_REVERSE});
    } while ((input = manage_cursor(wm->get_focused(), 1, 2, kMenuOpt)) != "");

    return 0;
}
