#include <clocale>
#include <csignal>
#include <curses.h>
#include <filesystem>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <random>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../include/XmlRecipeDeserializer.h"
#include "../include/CookBook.h"
#include "../include/WindowManager.h"

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

const std::string kVimKeys{"Comandi:\n \
    j -> Down;\n \
    k -> Up;\n \
    h -> Left;\n \
    l -> Right."};

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

// Stampa le entries nella finestra win a partire dalla posizione passsata.
// Le opzioni vengono stampate sulla stessa colonna.
// La numerazione delle opzioni parte dall'indice 1.
void display_menu_entries(const std::shared_ptr<gz::Window>& win, const std::vector<std::string>& entries, int y, int x) {
    std::ostringstream menu;
    int i{1};
    for (const std::string& e : entries)
        menu << "    "  << std::to_string(i++) + " - " << e << '\n';
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

// Gestine cursore per navigare i menu. Il cursore si muove lungo le ordinate
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
        }
    }
    win->display_refresh(" ", cur_y, start_x);
    return -1;
}

// Gestione opzione ricetta random.
void random_recipe_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // salvo le portate (non cambia durante l'esecuzione)
    static const std::vector<std::string> types{book.get_recipes_types()};

    // stampa menu di selezione portata
    win->clear_content();
    win->display("- Random - Scegli una delle seguenti portate:", 0, 0);
    display_menu_entries(win, types, 1, 0);

    // stampa ricetta random
    int selected{1};
    while ((selected = manage_cursor(win, selected, 2, types.size(), 1)) > 0) {
        // salvo le ricette della portata selezionata
        std::map<std::string, gz::Recipe> recipes{book.get_recipes(types[selected - 1])};

        // stampa ricetta casuale
        win->clear_content();
        auto it = recipes.begin();
        std::advance(it, random_int(0, recipes.size() - 1));
        display_recipe_info(win, it->second, 0, 0);

        // aspetto che che l'utente torni indietro dalla selezione
        for (char c{}; c != 'h'; c = win->get_ch()) /* wait */;
        win->clear_content();
        win->display("- Random - Scegli una delle seguenti portate:", 0, 0);
        display_menu_entries(win, types, 1, 0);
    }
    win->clear();
}
 
// Gestione menu di navigazione.
void navigate_opt(const std::shared_ptr<gz::Window>& win, const gz::CookBook& book) {
    // salvo le portate (non cambia durante l'esecuzione)
    static const std::vector<std::string> types{book.get_recipes_types()};

    // display menu iniziale
    win->clear_content();
    win->display("- Navigazione -", 0, 0);
    display_menu_entries(win, types, 1, 0);

    // gestione navigazione
    int input{1};
    while ((input = manage_cursor(win, input, 2, types.size(), 1)) > 0 ) {
        // salvo le ricette della portata selezionata
        std::map<std::string, gz::Recipe> recipes{book.get_recipes(types[input - 1])};

        // stampa ricette 
        win->clear_content();
        auto it = recipes.begin();
        display_recipe_info(win, it->second, 0, 0);
        int current_index{1}; // indice ricetta corrente
        win->display_refresh("[" + std::to_string(current_index) + "/" + std::to_string(recipes.size()) + "]", win->height() - 1, 0);
        for (char c{}; c != 'h'; c = win->get_ch()) {
            switch (c) {
            case 'j':
                if (++it != recipes.end()) {
                    win->clear_content();
                    display_recipe_info(win, it->second, 0, 0);
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
                    display_recipe_info(win, it->second, 0, 0);
                    --current_index;
                    win->display_refresh("[" + std::to_string(current_index) + "/" + std::to_string(recipes.size()) + "]", win->height() - 1, 0);
                }
                break;
            }
        }

        // display menu iniziale (se torno indietro dalla selezione)
        win->clear_content();
        win->display("- Navigazione -", 0, 0);
        display_menu_entries(win, types, 1, 0);
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
    int y{}; // ordinata corrente
    std::string input;
    while ((input = win->get_str()) != "q") {
        gz::Recipe found;
        if (( found = book.find(input)) != gz::Recipe{}) {
            win->clear_content();
            noecho();
            display_recipe_info(win, found, 0, 0);

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

// Stampa in win il menu principale.
void display_main_menu(const std::shared_ptr<gz::Window>& win, int y, int x) {
    win->clear_content();
    win->display_refresh("- Menu principale -", y, x);
    display_menu_entries(win, kMenuOpt, y+1, 0);
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


    // info box 
    //wm->create_win("menu", 8, 40, 0, 0)->set_border();
    //wm->get_focused()->display_refresh(kMenu, 1, 1);
    wm->create_win("vim keys", 8, 40, 0, 0)->set_border();
    wm->get_focused()->display_refresh(kVimKeys, 1, 1);

    // finestra principale
    wm->create_win("main", 38, 138, 9, 0);
    display_main_menu(wm->get_focused(), 0, 0);

    // gestione comandi
    int input{1};
    while ((input = manage_cursor(wm->get_focused(), input, 2, kMenuOpt.size(), 1)) > 0) {
        switch (input) {
        case 1:
            random_recipe_opt(wm->get_focused(), c_book);
            display_main_menu(wm->get_focused(), 0, 0);
            break;
        case 2:
            find_recipe_opt(wm->get_focused(), c_book);
            display_main_menu(wm->get_focused(), 0, 0);
            break;
        case 3:
            navigate_opt(wm->get_focused(), c_book);
            display_main_menu(wm->get_focused(), 0, 0);
            break;
        }
    }

    return 0;
}
