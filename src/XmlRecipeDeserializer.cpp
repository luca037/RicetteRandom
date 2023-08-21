#include "../include/XmlRecipeDeserializer.h"

namespace giallozafferano {

Recipe giallozafferano::XmlRecipeDeserializer::deserialize(const std::string& path) {
    Recipe r{};

    pugi::xml_parse_result res = doc_.load_file(path.c_str());
    pugi::xml_node head = doc_.child("Recipe");
    if (!head) 
        return r;
    r.set_name(head.child_value("Name"));
    r.set_ingredients(head.child_value("Ingredients"));
    r.set_preparation(head.child_value("Preparation"));
    return r;
}

} // end namespace
