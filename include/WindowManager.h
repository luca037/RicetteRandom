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

     // Crea una finestra. Ogni finestra è associata ad un nome.
     // Torna la finestra creata.
     // Il focus viene spostato sulla finestra creata.
     std::shared_ptr<Window> create_win(const std::string& name, int h=0, int l=0, int y=0, int x=0);

     // Torna la finestra che ha il foucs.
     std::shared_ptr<Window> get_focused() { 
         return focused_; 
     }

     // Cerca una finestra attraverso il nome.
     // Se la finestra è presente: torna ture e il focus viene spostato su tale
     // finestra. Se la finestra non è presente torna false.
     bool find_win(const std::string& name);

     // Gestione ridimensionamento terminale.
     static void handle_resize(int sig) {
         // reset ncurses env
         endwin();
         refresh();
         // init curses env
         initscr();
         // ridisegno tutte le finestre
         for (auto& p : windows_) {
             // pulizia finestra
             std::string cont = p.second->content();
             bool hb = p.second-> has_border();
             p.second->clear(); // pulisco tutto
             // ridisegno la finestra
             p.second->resize(p.second->height(), p.second->length());
             if (hb) p.second->set_border();
             if (!cont.empty()) p.second->display(cont.c_str(), 0, 0);
             p.second->refresh();
         }
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
