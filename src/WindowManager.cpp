#include "../include/WindowManager.h"

namespace giallozafferano {

std::shared_ptr<WindowManager> WindowManager::instance_ = nullptr;
std::map<std::string, Window> WindowManager::windows_;
Window* WindowManager::focused_ = nullptr;

/*! TODO: modificare la doppia ricerca non necessaria
 *  \todo modificare la doppia ricerca non necessaria
 */
void WindowManager::create_window(const std::string& name, int h, int l, int y, int x) {
    windows_[name] = Window{h, l, y, x};
    focused_ = &windows_[name];
}

void WindowManager::display_on_focused(const std::string& str, int y, int x) {
    if (!focused_) return;
    focused_->display(str.c_str(), y, x);
    focused_->refresh();
}

//void WindowManager::close_window(std::shared_ptr<Window> win) {
//    std::vector<std::shared_ptr<Window>>::iterator w = std::find(windows_.begin(), windows_.end(), win);
//    if (w != windows_.end())
//        windows_.erase(w);
//}

}
