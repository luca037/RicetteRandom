cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

project(Ricette LANGUAGES CXX)

find_package(Curses REQUIRED)

include_directories(third_party/pugixml)
include_directories(include)
include_directories(include ${CURSES_INCLUDE_DIR})

add_library(Ricette
    include/Window.h
    src/Window.cpp
    include/CookBook.h
    include/Recipe.h
    src/Recipe.cpp
    include/XmlRecipeDeserializer.h
    src/XmlRecipeDeserializer.cpp
    include/WindowManager.h
    src/WindowManager.cpp
    )

add_executable(ricetterandom
    src/ricetterandom.cpp
    )

target_link_libraries(ricetterandom pugixml)
target_link_libraries(ricetterandom Ricette)
target_link_libraries(ricetterandom Ricette ${CURSES_LIBRARIES})
