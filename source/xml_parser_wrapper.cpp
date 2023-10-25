#include "xml_parser_wrapper.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

XMLParserWrapper::XMLParserWrapper() {
    ERR_FAIL_COND(xmlParserWrapper != nullptr);
    xmlParserWrapper = this;
}

XMLParserWrapper::~XMLParserWrapper() {
    ERR_FAIL_COND(xmlParserWrapper != this);
    xmlParserWrapper = nullptr;
}

XMLParserWrapper *XMLParserWrapper::get_singleton() {
    return xmlParserWrapper;
}

Dictionary XMLParserWrapper::parse_file(const String& path) {
    Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
    PackedByteArray xml = file->get_as_text().to_utf8_buffer();
    Dictionary result = _parse(xml);
    return this->_to_dict(result);
}

Dictionary XMLParserWrapper::parse_string(const String& xml) {
    Dictionary result = _parse(xml.to_utf8_buffer());
    return this->_to_dict(result);
}

Dictionary XMLParserWrapper::parse_buffer(const PackedByteArray& xml) {
    Dictionary result = _parse(xml);
    return this->_to_dict(result);
}

void XMLParserWrapper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("parse_file", "path"), &XMLParserWrapper::parse_file);
    ClassDB::bind_method(D_METHOD("parse_string", "xml"), &XMLParserWrapper::parse_string);
    ClassDB::bind_method(D_METHOD("parse_buffer", "xml"), &XMLParserWrapper::parse_buffer);
}

XMLParserWrapper *XMLParserWrapper::xmlParserWrapper = nullptr;

Dictionary XMLParserWrapper::_parse(const PackedByteArray& xml) {
    Array queue = Array();

    Dictionary result = Dictionary();

    Ref<XMLParser> parser;
    parser.instantiate();
    parser->open_buffer(xml);

    while (parser->read() != ERR_FILE_EOF) {
        Variant node = _make_node(parser, queue);

        if (!node) {
            continue;
        }

        if (queue.size() == 0) {
            result = node;
            queue.append(node);
        } else {
            int node_type = parser->get_node_type();

            if (node_type == XMLParser::NODE_TEXT) {
                continue;
            }

            if (node.operator Dictionary()["standalone"] && node_type != XMLParser::NODE_ELEMENT_END) {
                queue.back().operator Dictionary()["children"].operator Array().append(node);
            } else if (node_type == XMLParser::NODE_ELEMENT_END && !node.operator Dictionary()["standalone"]) {
                queue.pop_back();
            } else {
                queue.back().operator Dictionary()["children"].operator Array().append(node);
                queue.append(node);
            }
        }
    }

    return result;
}

Variant XMLParserWrapper::_make_node(const Ref<XMLParser>& parser, Array &queue) {
    int node_type = parser->get_node_type();
    switch (node_type) {
        case XMLParser::NODE_ELEMENT: {
            Dictionary node = Dictionary();

            node["name"] = parser->get_node_name();
            node["attributes"] = _get_attributes(parser);
            node["content"] = "";
            node["standalone"] = parser->is_empty();
            node["children"] = Array();

            return node;
        }
        case XMLParser::NODE_ELEMENT_END: {
            Dictionary node = Dictionary();

            node["name"] = parser->get_node_name();
            node["attributes"] = Dictionary();
            node["content"] = "";
            node["standalone"] = false;
            node["children"] = Array();

            return node;
        }
        case XMLParser::NODE_TEXT: {
            Dictionary node = queue.back().operator Dictionary();
            if (node["content"].operator String().is_empty()) {
                node["content"] = parser->get_node_data().strip_edges().lstrip(" ").rstrip(" ");
            }
            return Variant::NIL;
        }
        default: {
            return Variant::NIL;
        }
    }
}

Dictionary XMLParserWrapper::_get_attributes(const Ref<XMLParser>& parser) {
    Dictionary attributes = Dictionary();
    int attributes_count = parser->get_attribute_count();

    for (int attribute_idx = 0; attribute_idx < attributes_count; attribute_idx++) {
        attributes[parser->get_attribute_name(attribute_idx)] = parser->get_attribute_value(attribute_idx);
    }
    return attributes;
}

Dictionary XMLParserWrapper::_to_dict(Dictionary node, bool include_empty_fields, const String& node_content_field_name) {
    Dictionary data = Dictionary();

    if (include_empty_fields) {
        data = _to_dict_all_fields(node);
        data["children"] = Array();
    } else {
        data = _to_dict_least_fields(node);
    }

    data[node_content_field_name] = node["content"];

    for (int child_idx = 0; child_idx < node["children"].operator Array().size(); child_idx++) {
        Dictionary child = node["children"].operator Array()[child_idx];
        if (!data.has("children")) {
            data["children"] = Array();
        }

        data["children"].operator Array().append(_to_dict(child));
    }

    return data;
}

Dictionary XMLParserWrapper::_to_dict_all_fields(Dictionary node) {
    Dictionary data = Dictionary();

    data["name"] = node["name"];
    data["attributes"] = node["attributes"];
    data["standalone"] = node["standalone"];

    return data;
}

Dictionary XMLParserWrapper::_to_dict_least_fields(Dictionary node) {
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