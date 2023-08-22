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
     //CookBook(const std::map<std::string, std::vector<Recipe>>& book) : {}

     CookBook(const CookBook&) = default;
     CookBook(CookBook&&) = default;
     CookBook& operator=(const CookBook&) = default;
     CookBook& operator=(CookBook&&) = default;

     // return book size (number of recipes)
     int size() const { return recipes_.size(); }

     // insert new recipe
     void insert(const std::string& type, const Recipe& r) { 
         recipes_[type].push_back(r);
     }

     // insert multipe recipes
     void insert(const std::string& type, const std::vector<Recipe>& rec) { 
         recipes_[type] = rec;
     }

     std::vector<Recipe> get_recipes(const std::string& type) {
         return recipes_[type];
     }

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
