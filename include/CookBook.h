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

     // mod
     void insert(const Recipe& r) { recipes_.push_back(r); }

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

 private:
     std::vector<Recipe> recipes_;
};

} // end namespace

#endif
