#ifndef XMLRECIPEDESERIALIZER_H
#define XMLRECIPEDESERIALIZER_H

#include <pugixml.hpp>
#include <string>
#include "Recipe.h"

namespace giallozafferano {

class XmlRecipeDeserializer {
 public:
     XmlRecipeDeserializer() {}

     Recipe deserialize(const std::string& path);

 private:
     pugi::xml_document doc_;
};

} // end namespace

#endif
