#include "../include/WindowManager.h"
#include <memory>

namespace giallozafferano {

std::shared_ptr<WindowManager> WindowManager::instance_ = nullptr;
std::map<std::string, std::shared_ptr<Window>> WindowManager::windows_;
std::shared_ptr<Window> WindowManager::focused_;

} // end namespace
