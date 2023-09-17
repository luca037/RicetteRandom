#include "../include/Window.h"

namespace giallozafferano {

Window::Window(int h, int l, int y, int x) 
    : height_{h},
      length_{l},
      y_pos_{y},
      x_pos_{x},
      last_content_{},
      has_border_{} {
        win_ = newwin(height_, length_, y_pos_, x_pos_);
}

Window::Window(const Window& w) :
    height_{w.height_}, length_{w.length_}, y_pos_{w.y_pos_}, x_pos_{w.x_pos_} {
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
}

Window& Window::operator=(const Window& w) {
    height_ = w.height_;
    length_ = w.length_;
    x_pos_ = w.x_pos_;
    y_pos_ = w.y_pos_;
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
    return *this;
}

Window::Window(Window&& w) :
    height_{w.height_}, length_{w.length_}, y_pos_{w.y_pos_}, x_pos_{w.x_pos_} {
    win_ = newwin(height_, length_, y_pos_, x_pos_); 
    w.height_ = 0;
    w.length_ = 0;
    w.x_pos_ = 0;
    w.y_pos_ = 0;
    w.win_ = nullptr;
}

Window& Window::operator=(Window&& w) {
    if (this != &w) {
        height_ = w.height_;
        length_ = w.length_;
        x_pos_ = w.x_pos_;
        y_pos_ = w.y_pos_;
        win_ = newwin(height_, length_, y_pos_, x_pos_); 
        w.height_ = 0;
        w.length_ = 0;
        w.x_pos_ = 0;
        w.y_pos_ = 0;
        w.win_ = nullptr;
    }
    return *this;
}

} // end namespace
