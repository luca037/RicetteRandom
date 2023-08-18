#ifndef XMLRECIPEDESERIALIZER_H
#define XMLRECIPEDESERIALIZER_H

#include <pugixml.hpp>
#include <string>
#include "Recipe.h"

namespace giallozafferano {

class XmlRecipeDeserializer {
 public:
     XmlRecipeDeserializer() {}

     XmlRecipeDeserializer(const XmlRecipeDeserializer&) = delete;
     XmlRecipeDeserializer(XmlRecipeDeserializer&&) = default;

     Recipe deserialize(const std::string& path);

     XmlRecipeDeserializer& operator=(const XmlRecipeDeserializer&) = delete;
     XmlRecipeDeserializer& operator=(XmlRecipeDeserializer&&) = default;

 private:
     pugi::xml_document doc_;
};

} // end namespace

#endif
