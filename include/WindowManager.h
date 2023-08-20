#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <memory>
#include <map>
#include <curses.h>
#include "Window.h"

namespace giallozafferano {

class WindowManager {
 public:
    static std::shared_ptr<WindowManager> get_instance() {
         if (!instance_) {
             instance_ = std::shared_ptr<WindowManager>{new WindowManager{}};
             //index_focus_ = 0;
         }
        return instance_;
     }

     void create_window(const std::string& name, int h=0, int l=0, int y=0, int x=0);
     void display_on_focused(const std::string& str, int y, int x);
     char getch_from_focused() const { if (!focused_) return 0; return focused_->get_ch(); }
     void clear_focused() { focused_->clear(); }

     //std::shared_ptr<Window> get_focused() const { return windows_[ }
     //void close_window(std::shared_ptr<Window> win);

    ~WindowManager() { endwin(); }

 private:
     WindowManager() { initscr(); }
     static std::shared_ptr<WindowManager> instance_;
     static std::map<std::string, Window> windows_;
     static Window* focused_;
};

}

#endif
