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

} // end namespace
