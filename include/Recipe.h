#ifndef RECIPE_H
#define RECIPE_H

#include <iomanip>
#include <ostream>
#include <string>

namespace giallozafferano {

// Classe che rappresenta una ricetta.
class Recipe {
 public:
    Recipe(const std::string& name="", const std::string& ing="", const std::string& prep="") :
        name_{name}, ingredients_{ing}, preparation_{prep} { }

    Recipe(const Recipe&) = default;
    Recipe(Recipe&&) = default;
    Recipe& operator=(const Recipe&) = default;
    Recipe& operator=(Recipe&&) = default;

    // Nome ricetta.
    std::string name() const { return name_; }

    // Ingredienti ricetta.
    std::string ingredients() const { return ingredients_; }

    // Preparazione ricetta.
    std::string preparation() const { return preparation_; }

    // Set name.
    void set_name(const std::string& name) { name_ = name; }

    // Set ingredients.
    void set_ingredients(const std::string& ing) { ingredients_ = ing; }

    // Set preparation.
    void set_preparation(const std::string& prep) { preparation_ = prep; }

 private:
    std::string name_;
    std::string ingredients_;
    std::string preparation_;
};

std::string tolower(const std::string& s);
bool operator==(const Recipe& lhs, const Recipe& rhs);
bool operator!=(const Recipe& lhs, const Recipe& rhs);

} // end namespace

#endif
