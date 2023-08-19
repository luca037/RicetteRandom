#ifndef COOKBOOK_H
#define COOKBOOK_H

#include "Recipe.h"
#include <ostream>
#include <vector>

namespace giallozafferano {

class CookBook {
 public:
     CookBook() {}
     CookBook(const std::vector<Recipe>& rec) : recipes_{rec} {}

     CookBook(const CookBook&) = default;
     CookBook(CookBook&&) = default;

     // return book size (number of recipes)
     int size() const { return recipes_.size(); }

     // insert new recipe
     void insert(const Recipe& r) { recipes_.push_back(r); }

     // get all recipes
     std::vector<Recipe> get_recipes() const { return recipes_; }

     friend std::ostream& operator<<(std::ostream& os, const CookBook& cb) {
         os << "[";
         for (int i = 0; i < cb.recipes_.size(); ++i) {
             if (i != (cb.recipes_.size() - 1))
                 os << cb.recipes_[i] << ",";
             else 
                 os << cb.recipes_[i] << "]";
         }
         return os;
     };

     CookBook& operator=(const CookBook&) = default;
     CookBook& operator=(CookBook&&) = default;

     ~CookBook() = default;

 private:
     std::vector<Recipe> recipes_;
};

} // end namespace

#endif
