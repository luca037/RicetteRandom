#include "../include/Window.h"
#include <curses.h>

namespace giallozafferano {

Window::Window(int h, int l, int y, int x) 
    : height_{h},
      length_{l},
      y_pos_{y},
      x_pos_{x},
      has_border_{} 
{
    win_ = newwin(height_, length_, y_pos_, x_pos_);
    content_ = std::vector(height_, std::vector<char>(length_));
}

Window::Window(const Window& w) 
    : height_{w.height_},
      length_{w.length_}, 
      y_pos_{w.y_pos_},
      x_pos_{w.x_pos_},
      has_border_{w.has_border_},
      content_{w.content_}
{
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
}

Window& Window::operator=(const Window& w) {
    height_ = w.height_;
    length_ = w.length_;
    y_pos_ = w.y_pos_;
    x_pos_ = w.x_pos_;
    has_border_ = w.has_border_;
    content_ = w.content_;
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
    return *this;
}

Window::Window(Window&& w) 
    : height_{w.height_},
      length_{w.length_}, 
      y_pos_{w.y_pos_},
      x_pos_{w.x_pos_},
      has_border_{w.has_border_},
      content_{w.content_}
{
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
    w.height_ = w.length_ = w.y_pos_ = w.x_pos_ = w.has_border_ = 0;
    w.content_ = {{}};
    w.win_ = nullptr;
}

Window& Window::operator=(Window&& w) {
    if (this != &w) {
        height_ = w.height_;
        length_ = w.length_;
        x_pos_ = w.x_pos_;
        y_pos_ = w.y_pos_;
        win_ = newwin(height_, length_, y_pos_, x_pos_); 
        content_ = w.content_;
        w.height_ = w.length_ = w.y_pos_ = w.x_pos_ = w.has_border_ = 0;
        w.win_ = nullptr;
        w.content_=  {{}};
    }
    return *this;
}

void Window::display(const std::string& str, int y, int x) { 
    mvwprintw(win_, y, x, "%s", str.c_str());
    // salvo la posizione del cursore
    int prev_x, prev_y;
    getyx(win_, prev_y, prev_x);
    if (has_border_) set_border();
    // aggiorno matrice contenuto
    for (int y = 0; y < height_; ++y)
        for (int x = 0; x < length_; ++x)
            content_[y][x] = mvwinch(win_, y, x);
    // reset posizione cursore
    move_cur(prev_y, prev_x);
}

void Window::redraw() const {
    // redraw content
    for (int y = 0; y < height_; ++y)
        for (int x = 0; x < length_; ++x)
            mvwprintw(win_, y, x, "%c", content_[y][x]);
    // reset size
    wresize(win_, height_, length_);
    // redraw border
    if (has_border_) wborder(win_, 0, 0, 0, 0, 0, 0, 0, 0);
}

} // end namespace
