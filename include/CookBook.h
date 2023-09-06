#ifndef COOKBOOK_H
#define COOKBOOK_H

#include "Recipe.h"
#include <algorithm>
#include <ostream>
#include <map>
#include <stdexcept>
#include <vector>
#include <string>

namespace giallozafferano {

class CookBook {
 public:
    CookBook() {}

    CookBook(const CookBook&) = default;
    CookBook(CookBook&&) = default;
    CookBook& operator=(const CookBook&) = default;
    CookBook& operator=(CookBook&&) = default;

    // Inserisce una nuova ricetta.
    void insert(const std::string& type, const Recipe& r) { 
        recipes_[type].push_back(r);
    }

    // Inserisce più ricette. Se esistono già delle ricette associate a type,
    // queste vengono sostiutie.
    void insert(const std::string& type, const std::vector<Recipe>& rec) { 
        recipes_[type] = rec;
    }

    // Ritorna una copia delle ricette associate a type.
    std::vector<Recipe> get_recipes(const std::string& type) const {
        try { return recipes_.at(type); } 
        catch (const std::out_of_range& e) { }
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
    Recipe find(const std::string name) const {
        for (auto const p : recipes_) {
            std::vector<Recipe>::const_iterator it = std::find(p.second.begin(), p.second.end(), Recipe{name});
            if (it != p.second.end())
                return *it;
        }
        return Recipe{};
    }

    ~CookBook() = default;

 private:
    std::map<std::string, std::vector<Recipe>> recipes_;
};

} // end namespace

#endif
