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
    c - Pulisci output;\n \
    n - Naviga ricettario;\n \
    q - Per uscire/annullare."

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
void display_types_menu(const std::shared_ptr<gz::Window>& win, const std::vector<std::string>& opts, int y, int x) {
    std::ostringstream menu;
    int i{1};
    for (const std::string& opt : opts)
        menu << "    "  << std::to_string(i++) + " - " << opt << '\n';
    win->display_refresh(menu.str(), y, x);
}

// Stampa le informazioni della ricetta sulla finestra passata partendo dalle 
// coordinate passate.
void display_recipe_info(const std::shared_ptr<gz::Window>& win, const gz::Recipe& recipe, int y, int x) {
    std::ostringstream oss;
    oss << "NOME: " << recipe.name() << '\n' << '\n'
        << "INGREDIENTI: " << recipe.ingredients() << '\n' << '\n'
        << "PREPARAZIONE: " << recipe.preparation();
    win->display_refresh(oss.str(), y, x);
}

// Gestine cursore per navigare i menu. Il cursore si muove verso lungo le ordinate
// usando j, k (vim). Per confermare la scelta si utilizza l, in tal caso viene 
// tornato il valore corrisponde alla posizione (ordinate) del cursore.
// Altrimenti, se premo:
//     q (annullare) -> torna -1
//     h (tornare inietro) -> torna -2
int manage_cursor(const std::shared_ptr<gz::Window>& win, int start_y, int start_x, int max_y, int min_y) {
    static const std::string cur{">"};
    int cur_y{start_y};
    win->display_refresh(cur, cur_y, start_x);
    // gestione movimento
    for (char input{}; input != 'q'; input = win->get_ch()) {
        switch (input) {
            case 'l': // right (select item)
                win->display_refresh(" ", cur_y, start_x);
                return cur_y;
                break;
            case 'h': // left (go back)
                win->display_refresh(" ", cur_y, start_x);
                return -2;
                break;
            case 'k': // up
                win->display(" ", cur_y, start_x);
                cur_y = cur_y == min_y ? max_y : cur_y - 1;
                win->display_refresh(">", cur_y, start_x);
                break;
            case 'j': // down
                win->display(" ", cur_y, start_x);
                cur_y = cur_y == max_y ? min_y : cur_y + 1;
                win->display_refresh(">", cur_y, start_x);
                break;
            defalut: continue;
        }
    }
    win->display_refresh(" ", cur_y, start_x);
    return -1;
}

// Gestione opzione ricetta random.
void random_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    static const std::vector<std::string> types_list{book.get_recipes_types()};
    // stampa menu di selezione portata
    win->clear_content();
    win->display("- Random - Scegli una delle seguenti portate:", 0, 0);
    display_types_menu(win, types_list, 1, 0);
    int selected{manage_cursor(win, 1, 2, types_list.size(), 1)};
    // stampa ricetta random
    if (selected > 0) {
        std::vector<gz::Recipe> rec = book.get_recipes(types_list[selected - 1]);
        win->clear_content();
        display_recipe_info(win, rec[random_int(0, rec.size() - 1)], 0, 0);
    }
}

// Gestione menu di navigazione.
void navitate_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    static const std::vector<std::string> types_list{book.get_recipes_types()};
    // display menu iniziale
    win->clear_content();
    win->display("- Navigazione -", 0, 0);
    display_types_menu(win, types_list, 1, 0);
    // gestione navigazione
    int input;
    while ((input = manage_cursor(win, 1, 2, types_list.size(), 1)) > 0 ) {
        // stampa ricetta della portata selezionata
        std::vector<gz::Recipe> rec{book.get_recipes(types_list[input - 1])};
        win->clear_content();
        display_recipe_info(win, rec[0], 0, 0);
        for (int c{}, i{}; c != 'h'; c = win->get_ch()) {
            if (char(c) == 'j') {
                if (i + 1 < rec.size()) { // stampa ricetta successiva
                    ++i;
                    win->clear_content();
                    display_recipe_info(win, rec[i], 0, 0);
                }
            } else if (char(c) == 'k') {
                if (i - 1 > 0) { // stampa ricetta precedente
                    --i;
                    win->clear_content();
                    display_recipe_info(win, rec[i], 0, 0);
                }
            }
        }
        // display menu iniziale (se torno indietro dalla selezione0)
        win->clear_content();
    win->display("- Navigazione -", 0, 0);
        display_types_menu(win, types_list, 1, 0);
    }
    win->clear();
}

// Gestione ricerca ricetta e stampa risultati.
void find_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // inizializzo ambiente
    echo();
    curs_set(1);
    win->clear_content();
    win->display_refresh(">> inserire nome: ", 0, 0);
    // ricerca ricetta
    int y{};
    std::string input;
    while ((input = win->get_str()) != "q") {
        gz::Recipe found;
        if (( found = book.find(input)) != gz::Recipe{}) {
            win->clear_content();
            noecho();
            display_recipe_info(win, found, 0, 0);
            for (char c{}; c != 'h'; c = win->get_ch()) /* wait */;
            win->clear_content();
            win->display_refresh(">> inserire nome: ", 0, 0);
            y = 0;
            echo();
        } else {
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
    wm->create_win("menu", 8, 40, 0, 142)->set_border();
    wm->get_focused()->display_refresh(kMenu, 1, 1);

    // finestra principale
    wm->create_win("border-main", 40, 140, 0, 0)->set_border(); // funge solo da bordo
    wm->get_focused()->refresh();
    wm->create_win("main", 38, 138, 1, 1);

    // start applicazione
    for (char input{}; input != 'q'; input = wm->get_focused()->get_ch()) {
        switch (input) {
            case '1':
                random_recipe_opt(wm->get_focused(), c_book);
                break;
            case '2':
                find_recipe_opt(wm->get_focused(), c_book);
                break;
            case 'c':
                wm->get_focused()->clear_content();
                break;
            case 'n':
                navitate_opt(wm->get_focused(), c_book);
                break;
            default: continue;
        }
    }

    return 0;
}
