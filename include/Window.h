#ifndef WINDOW_H
#define WINDOW_H

#include <cstdlib>
#include <curses.h>
#include "Recipe.h"

namespace giallozafferano {

// Classe che rappresenta una finestra.
class Window {
 public:
    Window(int h=0, int l=0, int y=0, int x=0) : 
        height_{h}, length_{l}, y_pos_{y}, x_pos_{x}, content_{}, has_border_{} { 
            win_ = newwin(height_, length_, y_pos_, x_pos_); 
        }

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

    // Torna il contenuto della finestra (ultima stringa stampata con display o
    // display_refresh).
    std::string content() const { return content_; }

    // Disegna il bordo alla finestra.
    void set_border() { 
        wborder(win_, 0, 0, 0, 0, 0, 0, 0, 0);
        has_border_ = true; 
    }

    // Rimuove il bordo alla finestra al prossimo display.
    void remove_border() {
        has_border_ = false;
    }

    // Scrive str nella finestra a partire dalle coordinate passate.
    void display(const std::string& str, int y, int x) { 
        mvwprintw(win_, y, x, "%s", str.c_str());
        if (has_border_) set_border();
        content_ = str; 
    }

    // Cancella il contenuto della finestra e il bordo.
    void clear() { 
        wclear(win_); 
        content_ = "";
        has_border_ = false; 
    }

    // Cancella il contenuto della finestra. (Diverso da clear, vedi "man werase")
    void erase() {
        werase(win_);
        content_ = "";
        has_border_ = false;
    }

    // Cancella il contenuto, mantiene il bordo.
    void clear_content() {
        wclear(win_);
        if (has_border_) set_border();
    }

    // Refresh finestra.
    void refresh() { wrefresh(win_); }

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

    // Ridimensiona la finestra.
    void resize(int y, int x) {
        wresize(win_, y, x);
        height_ = y;
        length_ = x;
    }

    // Sposta la finestra.
    void move(int y, int x) {
        mvwin(win_, y, x);
        y_pos_ = y;
        x_pos_ = x;
    }

    ~Window() { delwin(win_); }

 private:
    WINDOW* win_;
    int height_;
    int length_;
    int x_pos_;
    int y_pos_;
    bool has_border_;
    std::string content_;
};
 
} // end namespace

#endif
