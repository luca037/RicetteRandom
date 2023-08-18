cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

project(Ricette LANGUAGES CXX)

include_directories(third_party/pugixml)

include_directories(include)

add_library(Ricette
    include/CookBook.h
    include/Recipe.h
    include/XmlRecipeDeserializer.h
    src/XmlRecipeDeserializer.cpp
    )

add_executable(ricetterandom
    src/ricetterandom.cpp
    )
target_link_libraries(ricetterandom pugixml)

target_link_libraries(ricetterandom Ricette)
