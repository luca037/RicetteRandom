#ifndef WINDOW_H
#define WINDOW_H

#include "Recipe.h"
#include <cstdlib>
#include <curses.h>

namespace giallozafferano {

class Window {
 public:
     Window(int h=0, int l=0, int y=0, int x=0) : 
         height_{h}, length_{l}, y_pos_{y}, x_pos_{x} { 
             win_ = newwin(height_, length_, y_pos_, x_pos_); 
         }

     Window(const Window& w);
     Window(Window&& w);
     Window& operator=(const Window& w);
     Window& operator=(Window&& w);

     void set_height(int h) { height_ = h; }
     void set_length(int l) { length_ = l; }
     int height() const { return height_; }
     int length() const { return length_; }
     std::string content() const { return content_; }

     void set_border() { wborder(win_, 0, 0, 0, 0, 0, 0, 0, 0); }
     void display(const char* str, int y, int x) { mvwprintw(win_, y, x, "%s", str); content_ = str; }
     void clear() { wclear(win_); content_ = ""; }
     void erase() { werase(win_); content_ = ""; }
     void refresh() { wrefresh(win_); }
     void display_refresh(const char* str, int y, int x) { display(str, y, x); refresh(); }
     char get_ch() const { return wgetch(win_); }
     void resize(int y, int x) { wresize(win_, y, x); }

     ~Window() { delwin(win_); }

 private:
     WINDOW* win_;
     int height_;
     int length_;
     int x_pos_;
     int y_pos_;
     std::string content_;
};
 
} // end namespace

#endif
