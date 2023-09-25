#ifndef WINDOW_H
#define WINDOW_H

#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <vector>
#include "Recipe.h"

namespace giallozafferano {

// Classe che rappresenta una finestra.
class Window {
 public:
    Window(int h, int l, int y, int x);

    Window(const Window& w);
    Window(Window&& w);
    Window& operator=(const Window& w);
    Window& operator=(Window&& w);

    // Ritorna l'altezza.
    int height() const { return height_; }

    // Ritorna la lunghezza.
    int length() const { return length_; }

    // Torna true se la finestra ha il bordo.
    bool has_border() const { return has_border_; }

    // Disegna il bordo alla finestra.
    void set_border() { 
        wborder(win_, 0, 0, 0, 0, 0, 0, 0, 0);
        has_border_ = true; 
    }

    // Rimuove il bordo alla finestra al prossimo display.
    void remove_border() { has_border_ = false; }

    // Scrive str nella finestra a partire dalle coordinate passate.
    void display(const std::string& str, int y, int x);

    // Cancella il contenuto della finestra e il bordo.
    void clear() { 
        wclear(win_); 
        content_ = std::vector(height_, std::vector<char>(length_));
        has_border_ = false; 
    }

    // Cancella il contenuto, mantiene il bordo.
    void clear_content() {
        wclear(win_);
        content_ = std::vector(height_, std::vector<char>(length_));
        if (has_border_) set_border();
    }

    // Refresh finestra.
    void refresh() const { wrefresh(win_); }

    // Ristampa contenuto.
    void redraw() const;

    // Display poi refresh finestra.
    void display_refresh(const std::string& str, int y, int x) {
        display(str, y, x);
        refresh(); 
    }

    // Input untente: ritorna il carattere inserito.
    char get_ch() const { return wgetch(win_); }

    // Input utente: ritorna la stringa inserita.
    std::string get_str() const {
        char str[256];
        wgetnstr(win_, str, sizeof(str));
        return str;
    }

    // Posiziona il cursore alle coordinate specificate.
    void move_cur(int y, int x) { wmove(win_, y, x); }

    ~Window() { delwin(win_); }

 private:
    WINDOW* win_;
    int height_;
    int length_;
    int x_pos_;
    int y_pos_;
    bool has_border_;
    std::vector<std::vector<char>> content_;
};
 
} // end namespace

#endif
