#include <algorithm>
#include <random>
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
// tornato il valore corrispondente all'elemento selezionato.
// Altrimenti, se premo 'h' o 'q' ritorna l'elemento zero della classe T.
template <typename T>
T manage_cursor(const std::shared_ptr<gz::Window>& win, int start_y, int start_x, const std::vector<T>& entries) {
    // stampa delle rimanenti opzioni
    int i{start_y};
    for (const std::string& e : entries) {
        win->display(e, i, start_x);
        ++i;
    }

    // evidenzio il primo elemento del menu
    win->attribute_on(A_REVERSE);
    win->display_refresh(entries.front(), start_y, start_x);
    win->attribute_off(A_REVERSE);
    
    int cur_y{start_y}; // ordinata elemento selezionato
    auto selected{entries.begin()}; // elemento selezionato

    // gestione movimento
    for (char input{}; input != 'q' && input != 'h'; input = win->get_ch()) {
        switch (input) {
        case 'l': // right (select item)
            win->display_refresh(*selected, cur_y, start_x);
            return *selected;
            break;
        case 'k': // up
            win->display(*selected, cur_y, start_x);
            if (selected != entries.begin()) {
                --selected;
                --cur_y;
            } else {
                selected = entries.end() - 1;
                cur_y = start_y + entries.size() - 1;
            }
            // highlight next item
            win->attribute_on(A_REVERSE);
            win->display_refresh(*selected, cur_y, start_x);
            win->attribute_off(A_REVERSE);
            break;
        case 'j': // down
            win->display(*selected, cur_y, start_x);
            if (++selected != entries.end()) {
                ++cur_y;
            } else {
                selected = entries.begin();
                cur_y = start_y;
            }
            // highlight next item
            win->attribute_on(A_REVERSE);
            win->display_refresh(*selected, cur_y, start_x);
            win->attribute_off(A_REVERSE);
            break;
        }
    }
    return {};
}
