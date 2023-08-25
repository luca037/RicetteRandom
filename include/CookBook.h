#ifndef COOKBOOK_H
#define COOKBOOK_H

#include "Recipe.h"
#include <ostream>
#include <map>
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

    // Ritorna le ricette associate a type.
    std::vector<Recipe> get_recipes(const std::string& type) {
        return recipes_[type];
    }

    // Ritorna un vettore contenente le tipologie delle ricette.
    std::vector<std::string> get_recipes_types() const {
        std::vector<std::string> keys;
        for (const auto& e : recipes_)
            keys.push_back(e.first);
        return keys;
    }

    ~CookBook() = default;

 private:
    std::map<std::string, std::vector<Recipe>> recipes_;
};

} // end namespace

#endif
