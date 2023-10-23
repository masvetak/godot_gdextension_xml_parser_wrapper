#include "xml_document.hpp"

using namespace godot;

XMLDocument::XMLDocument() {
    this->root = Dictionary();
}

XMLDocument::~XMLDocument() = default;

Dictionary XMLDocument::to_dict(bool include_empty_fields, const String& node_content_field_name) const {
    return m_to_dict(root, include_empty_fields, node_content_field_name);
}

Dictionary XMLDocument::m_to_dict(Dictionary node, bool include_empty_fields, const String& node_content_field_name) {
    Dictionary data = Dictionary();

    if (include_empty_fields) {
        data = m_to_dict_all_fields(node);
        data["children"] = Array();
    } else {
        data = m_to_dict_least_fields(node);
    }

    data[node_content_field_name] = node["content"];

    for (int child_idx = 0; child_idx < node["children"].operator Array().size(); child_idx++) {
        Dictionary child = node["children"].operator Array()[child_idx];
        if (!data.has("children")) {
            data["children"] = Array();
        }

        data["children"].operator Array().append(m_to_dict(child));
    }

    return data;
}

Dictionary XMLDocument::m_to_dict_all_fields(Dictionary node) {
    Dictionary data = Dictionary();

    data["name"] = node["name"];
    data["attributes"] = node["attributes"];
    data["standalone"] = node["standalone"];

    return data;
}

Dictionary XMLDocument::m_to_dict_least_fields(Dictionary node) {
    Dictionary data = Dictionary();

    if (!node["name"].operator String().is_empty()) {
        data["name"] = node["name"];
    }

    if (!node["attributes"].operator Dictionary().is_empty()) {
        data["attributes"] = node["attributes"];
    }

    data["standalone"] = node["standalone"];

    return data;
}
