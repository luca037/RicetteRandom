#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <memory>
#include <map>
#include <curses.h>
#include <string>
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

     static void handle_resize(int sig) {
         endwin();
         refresh();

         initscr();

         // ridisegno tutte le finestre
         for (auto& p : windows_) {
             std::string cont = p.second->content(); // salvo il contenuto
             p.second->clear(); // pulisco tutto
             p.second->resize(p.second->height(), p.second->length()); // risetto la dimesione
             if (p.first == "bg-border") p.second->set_border();
             else  p.second->display(cont.c_str(), 0, 0);
             p.second->refresh();
         }

         // Ridisegna l'interfaccia adattata alle nuove dimensioni
         //int maxY, maxX;
         //getmaxyx(stdscr, maxY, maxX);

         //WINDOW* win = newwin(20, 40, 0, 0);
         //wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
         //mvwprintw(win, 0, 0, "Terminal Resized!");

         //wrefresh(win);
     }


     ~WindowManager() { endwin(); }

 private:
     WindowManager() { initscr(); setlocale(LC_ALL, ""); noecho(); }
     static std::shared_ptr<WindowManager> instance_;
     static std::map<std::string, std::shared_ptr<Window>> windows_;
     static std::shared_ptr<Window> focused_;
};

}

#endif
