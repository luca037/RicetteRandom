#include "../include/XmlRecipeDeserializer.h"

namespace giallozafferano {

Recipe giallozafferano::XmlRecipeDeserializer::deserialize(const std::string& path) {
    pugi::xml_parse_result res = doc_.load_file(path.c_str());
    pugi::xml_node head = doc_.child("Recipe");
    if (!head) return {};
    return { 
        head.child_value("Name"),
        head.child_value("Ingredients"),
        head.child_value("Preparation")
    };
}

} // end namespace
