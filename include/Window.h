#ifndef WINDOW_H
#define WINDOW_H

#include "Recipe.h"
#include <cstdlib>
#include <curses.h>

namespace giallozafferano {

class Window {
 public:
     Window(int h, int l, int y, int x) : 
         height_{h}, length_{l}, y_pos_{y}, x_pos_{x} { 
             win_ = newwin(height_, length_, y_pos_, x_pos_); 
         }

     // mod
     void display(const char* str, int y, int x) {
         mvwprintw(win_, y, x, "%s", str);
     }
     void clear() { wclear(win_); }
     void refresh() { wrefresh(win_); }

     void set_height(int h) { height_ = h; }
     void set_length(int l) { length_ = l; }

     // non mod
     int height(int h) const { return height_; }
     int length(int l) const { return length_; }

     char get_ch() const { return wgetch(win_); }

     ~Window() { delwin(win_); }

 private:
     WINDOW* win_;
     int height_;
     int length_;
     int x_pos_;
     int y_pos_;
};

inline void initenv() { initscr(); }
inline void endenv() { endwin(); }
 
} // end namespace

#endif
