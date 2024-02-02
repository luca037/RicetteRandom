#include <algorithm>
#include <random>
#include <sstream>
#include <memory>

#include "../include/Window.h"
#include "../include/Recipe.h"

namespace gz = giallozafferano;

// Torna un numero random intero compreso nel range [min, max].
int random_int(int min, int max) {
    std::random_device rd;                           // random number from hardware
    std::mt19937 gen(rd());                          // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define range
    return distr(gen);
}

// Stampa le entries nella finestra win a partire dalla posizione passsata.
// Le opzioni vengono stampate sulla stessa colonna.
// La numerazione delle opzioni parte dall'indice 1.
void display_menu_entries(const std::shared_ptr<gz::Window>& win, const std::vector<std::string>& entries, int y, int x) {
    std::ostringstream menu;
    for (const std::string& e : entries)
        menu << "    "  << e << '\n';
    win->display_refresh(menu.str(), y, x);
}

// Stampa il menu principale.
void display_main_menu(const std::shared_ptr<gz::Window>& win, const std::vector<std::string>& entries,int y, int x) {
    win->clear_content();
    win->attribute_on({A_BOLD, A_REVERSE});
    win->display("- Menu principale -", 0, 0);
    win->attribute_off({A_BOLD, A_REVERSE});
    display_menu_entries(win, entries, y+1, 0);
}


// Stampa le informazioni della ricetta sulla finestra passata.
void display_recipe_info(const std::shared_ptr<gz::Window>& win, const gz::Recipe& recipe) {
    // titolo ricetta in maiuscolo
    std::string upper_n{recipe.name()};
    std::transform(upper_n.begin(), upper_n.end(), upper_n.begin(),
            [](unsigned char c) { return std::toupper(c); }
            );

    // stampa titolo
    win->attribute_on({A_BOLD, A_REVERSE});
    win->display(upper_n, 0, 0);
    win->attribute_off(A_REVERSE);

    // stampa ingredienti
    win->display("INGREDIENTI", 2, 0);
    win->attribute_off(A_BOLD);
    win->display(recipe.ingredients(), 3, 0);

    // stampa preparazione
    win->attribute_on(A_BOLD);
    win->display("PREPARAZIONE", 8, 0);
    win->attribute_off(A_BOLD);
    win->display(recipe.preparation(), 9, 0);
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
