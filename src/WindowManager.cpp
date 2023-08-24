#include "../include/WindowManager.h"
#include <memory>

namespace giallozafferano {

std::shared_ptr<WindowManager> WindowManager::instance_ = nullptr;
std::map<std::string, std::shared_ptr<Window>> WindowManager::windows_;
std::shared_ptr<Window> WindowManager::focused_;

std::shared_ptr<Window> WindowManager::create_win(const std::string& name, int h, int l, int y, int x) {
    focused_ = std::make_shared<Window>(h, l, y, x);
    windows_[name] = focused_;
    return focused_;
}

bool WindowManager::find_win(const std::string& name) {
    auto ptr = windows_.find(name);
    if (ptr != windows_.end()) {
        focused_ = ptr->second;
        return true;
    }
    return false;
}

void WindowManager::handle_resize(int sig) {
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

} // end namespace
