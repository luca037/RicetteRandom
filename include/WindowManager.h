#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <map>
#include <curses.h>
#include <string>
#include <memory>
#include "Window.h"

namespace giallozafferano {

// Classe che rappresenta il gestore delle finestre.
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
    std::shared_ptr<Window> create_win(const std::string& name, int h, int l, int y, int x) {
        focused_ = std::make_shared<Window>(h, l, y, x);
        windows_[name] = focused_;
        return focused_;
    }

    // Torna la finestra che ha il foucs.
    std::shared_ptr<Window> get_focused() { 
        return focused_; 
    }

    // Cerca una finestra attraverso il nome.
    // Se la finestra è presente: torna ture e il focus viene spostato su tale
    // finestra. Se la finestra non è presente torna false.
    bool find_win(const std::string& name) {
        auto ptr = windows_.find(name);
        if (ptr != windows_.end()) {
            focused_ = ptr->second;
            return true;
        }
        return false;
    }

    // Gestione ridimensionamento terminale.
    static void handle_resize(int sig) {
        // reset ncurses env
        endwin();
        refresh();
        // init curses env
        initscr();
        // ridisegno tutte le finestre
        for (auto& p : windows_) {
            p.second->redraw();
            p.second->refresh();
        }
    }

    ~WindowManager() { endwin(); }

private:
    WindowManager() { initscr(); }
    static std::shared_ptr<WindowManager> instance_;
    static std::map<std::string, std::shared_ptr<Window>> windows_;
    static std::shared_ptr<Window> focused_;
};

} // end namespace

#endif
