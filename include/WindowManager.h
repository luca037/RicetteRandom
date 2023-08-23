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
         if (!instance_)
             instance_ = std::shared_ptr<WindowManager>{new WindowManager{}};
         return instance_;
     }

     WindowManager(const WindowManager&) = delete;
     WindowManager(WindowManager&&) = delete;
     WindowManager& operator=(const WindowManager&) = delete;
     WindowManager& operator=(WindowManager&&) = delete;

     std::shared_ptr<Window> create_win(const std::string& name, int h=0, int l=0, int y=0, int x=0);
     std::shared_ptr<Window> get_focused() { return focused_; }
     bool find_win(const std::string& name);

     ~WindowManager() { endwin(); }

 private:
     WindowManager() { initscr(); setlocale(LC_ALL, ""); noecho(); }
     static std::shared_ptr<WindowManager> instance_;
     static std::map<std::string, std::shared_ptr<Window>> windows_;
     static std::shared_ptr<Window> focused_;
};

}

#endif
