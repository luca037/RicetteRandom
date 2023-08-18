#ifndef RECIPE_H
#define RECIPE_H

#include <iomanip>
#include <ostream>
#include <string>

namespace giallozafferano {

class Recipe {
 public:
     Recipe(const std::string& name="", const std::string& ing="", const std::string& prep="") :
         name_{name}, ingredients_{ing}, preparation_{prep} { }

     Recipe(const Recipe&) = default;
     Recipe(Recipe&&) = default;

     // non mod
     std::string name() const { return name_; }
     std::string ingredients() const { return ingredients_; }
     std::string preparation() const { return preparation_; }

     // mod
     void set_name(const std::string& name) { name_ = name; }
     void set_ingredients(const std::string& ing) { ingredients_ = ing; }
     void set_preparation(const std::string& prep) { preparation_ = prep; }

     friend bool operator==(const Recipe& lhs, const Recipe& rhs) { 
         return lhs.name() == rhs.name(); 
     }
     friend bool operator!=(const Recipe& lhs, const Recipe& rhs) { 
         return lhs.name() != rhs.name(); 
     }
     friend std::ostream& operator<<(std::ostream& os, const Recipe& r) {
         return os << "{" 
                   << std::quoted("Name") << ": " << std::quoted(r.name_) << ", "
                   << std::quoted("Ingredients") << ": " << std::quoted(r.ingredients_) << ", "
                   << std::quoted("Preparation") << ": " << r.preparation_ 
                   << "}";
     }

     Recipe& operator=(const Recipe&) = default;
     Recipe& operator=(Recipe&&) = default;

 private:
    std::string name_;
    std::string ingredients_;
    std::string preparation_;
};


} // end namespace

#endif
