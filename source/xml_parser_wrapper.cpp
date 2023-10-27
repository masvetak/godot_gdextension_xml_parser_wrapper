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
    return _parse(xml);
}

Dictionary XMLParserWrapper::parse_string(const String& xml) {
    return _parse(xml.to_utf8_buffer());
}

void XMLParserWrapper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("parse_file", "path"), &XMLParserWrapper::parse_file);
    ClassDB::bind_method(D_METHOD("parse_string", "xml"), &XMLParserWrapper::parse_string);
}

XMLParserWrapper *XMLParserWrapper::xmlParserWrapper = nullptr;

Dictionary XMLParserWrapper::_parse(const PackedByteArray& xml) {
    vector<xml_node_t> queue;

    xml_node_t result;

    Ref<XMLParser> parser;
    parser.instantiate();
    parser->open_buffer(xml);

    while (parser->read() != ERR_FILE_EOF) {
        xml_node_t node;
        int node_type = parser->get_node_type();
        if (node_type == XMLParser::NODE_ELEMENT) {
            node.name = parser->get_node_name();
            node.attributes = _get_attributes(parser);
            node.content = "";
            node.standalone = parser->is_empty();
            node.children = vector<xml_node_t>();
        } else if (node_type == XMLParser::NODE_ELEMENT_END) {
            node.name = parser->get_node_name();
            node.attributes = Dictionary();
            node.content = "";
            node.standalone = false;
            node.children = vector<xml_node_t>();
        } else if (node_type == XMLParser::NODE_TEXT) {
            node = queue.back();
            if (node.content.is_empty()) {
                node.content = parser->get_node_data().strip_edges().lstrip(" ").rstrip(" ");
            }
            continue;
        } else {
            continue;
        }

        UtilityFunctions::print(_to_dict(node));

        if (queue.empty()) {
            result = node;
            queue.push_back(node);
        } else {
            node_type = parser->get_node_type();

            if (node_type == XMLParser::NODE_TEXT) {
                continue;
            }

            if (node.standalone && node_type != XMLParser::NODE_ELEMENT_END) {
                xml_node_t &node_back = queue.back();
                node_back.children.push_back(node);
            } else if (node_type == XMLParser::NODE_ELEMENT_END && !node.standalone) {
                queue.pop_back();
            } else {
                xml_node_t &node_back = queue.back();
                node_back.children.push_back(node);
                queue.push_back(node);
            }
        }
    }

    return _to_dict(result);
}

Dictionary XMLParserWrapper::_get_attributes(const Ref<XMLParser>& parser) {
    Dictionary attributes = Dictionary();
    int attributes_count = parser->get_attribute_count();

    for (int attribute_idx = 0; attribute_idx < attributes_count; attribute_idx++) {
        attributes[parser->get_attribute_name(attribute_idx)] = parser->get_attribute_value(attribute_idx);
    }
    return attributes;
}

Dictionary XMLParserWrapper::_to_dict(const xml_node_t& node, bool include_empty_fields, const String& node_content_field_name) {
    Dictionary data = Dictionary();

    if (include_empty_fields) {
        data = _to_dict_all_fields(node);
        data["children"] = Array();
    } else {
        data = _to_dict_least_fields(node);
    }

    data[node_content_field_name] = node.content;

    for (xml_node_t child : node.children) {
        UtilityFunctions::print(child.name);
        if (!data.has("children")) {
            data["children"] = Array();
        }
        data["children"].operator Array().append(_to_dict(child));
    }

    return data;
}

Dictionary XMLParserWrapper::_to_dict_all_fields(const xml_node_t& node) {
    Dictionary data = Dictionary();

    data["name"] = node.name;
    data["attributes"] = node.attributes;
    data["standalone"] = node.standalone;

    return data;
}

Dictionary XMLParserWrapper::_to_dict_least_fields(const xml_node_t& node) {
    Dictionary data = Dictionary();

    if (!node.name.is_empty()) {
        data["name"] = node.name;
    }
    if (!node.attributes.is_empty()) {
        data["attributes"] = node.attributes;
    }
    data["standalone"] = node.standalone;

    return data;
}