#ifndef COOKBOOK_H
#define COOKBOOK_H

#include "Recipe.h"
#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <map>
#include <vector>
#include <string>

namespace giallozafferano {

// Classe che rappresenta un libro di ricette.
class CookBook {
 public:
    CookBook() {}

    CookBook(const CookBook&) = default;
    CookBook(CookBook&&) = default;
    CookBook& operator=(const CookBook&) = default;
    CookBook& operator=(CookBook&&) = default;

    // Inserisce una nuova ricetta.
    void insert(const std::string& type, const Recipe& r) { 
        auto it = std::find_if(recipes_.begin(), recipes_.end(),
                [&type](const std::pair<std::string, std::map<std::string, Recipe>>& p) {
                    return p.first == type;
                });
        if (it != recipes_.end())
            it->second[tolower(r.name())] = r;
        else
            recipes_.push_back(
                    std::make_pair(type, std::map<std::string, Recipe>{{tolower(r.name()), r}})
            );
    }

    // Ritorna una copia delle ricette associate a type.
    std::map<std::string, Recipe> get_recipes(const std::string& type) const {
        auto it = std::find_if(recipes_.begin(), recipes_.end(),
                [&type](const std::pair<std::string, std::map<std::string, Recipe>>& p) {
                    return p.first == type;
                });
        if (it != recipes_.end())
            return it->second;
        return {};
    }

    // Ritorna un vettore contenente le tipologie delle ricette.
    std::vector<std::string> get_recipes_types() const {
        std::vector<std::string> keys;
        for (const auto& e : recipes_)
            keys.push_back(e.first);
        return keys;
    }

    // Ricerca la ricetta per nome. Se non è presente nessuna ricetta torna una 
    // ricetta vuota.
    Recipe find(const std::string& name) const {
        for (const auto& p : recipes_) {
           auto it = p.second.find(tolower(name));
           if (it != p.second.end())
               return it->second;
        }
        return Recipe{};
    }

    ~CookBook() = default;

 private:
    std::vector<std::pair<std::string, std::map<std::string, Recipe>>> recipes_;
};

} // end namespace

#endif
