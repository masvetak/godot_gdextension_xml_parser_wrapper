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
    XMLDocument doc = m_parse(xml);
    return doc.to_dict();
}

Dictionary XMLParserWrapper::parse_string(const String& xml) {
    XMLDocument doc = m_parse(xml.to_utf8_buffer());
    return doc.to_dict();
}

Dictionary XMLParserWrapper::parse_buffer(const PackedByteArray& xml) {
    XMLDocument doc = m_parse(xml);
    return doc.to_dict();
}

void XMLParserWrapper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("parse_file", "path"), &XMLParserWrapper::parse_file);
    ClassDB::bind_method(D_METHOD("parse_string", "xml"), &XMLParserWrapper::parse_string);
    ClassDB::bind_method(D_METHOD("parse_buffer", "xml"), &XMLParserWrapper::parse_buffer);
}

XMLParserWrapper *XMLParserWrapper::xmlParserWrapper = nullptr;

XMLDocument XMLParserWrapper::m_parse(const PackedByteArray& xml) {
    XMLDocument doc;
    Array queue = Array();

    XMLParser parser;
    parser.open_buffer(xml);

    while (parser.read() != ERR_FILE_EOF) {
        Variant node = m_make_node(queue, parser);

        if (!node) {
            continue;
        }

        if (queue.size() == 0) {
            doc.root = node;
            queue.append(node);
        } else {
            int node_type = parser.get_node_type();

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

    return doc;
}

Variant XMLParserWrapper::m_make_node(Array &queue, XMLParser parser) {
    int node_type = parser.get_node_type();
    switch (node_type) {
        case XMLParser::NODE_ELEMENT: {
            Dictionary node = Dictionary();

            node["name"] = parser.get_node_name();
            node["attributes"] = m_get_attributes(parser);
            node["content"] = "";
            node["standalone"] = parser.is_empty();
            node["children"] = Array();

            return node;
        }
        case XMLParser::NODE_ELEMENT_END: {
            Dictionary node = Dictionary();

            node["name"] = parser.get_node_name();
            node["attributes"] = Dictionary();
            node["content"] = "";
            node["standalone"] = false;
            node["children"] = Array();

            return node;
        }
        case XMLParser::NODE_TEXT: {
            Dictionary node = queue.back().operator Dictionary();
            if (node["content"].operator String().is_empty()) {
                node["content"] = parser.get_node_data().strip_edges().lstrip(" ").rstrip(" ");
            }
            return Variant::NIL;
        }
        default: {
            return Variant::NIL;
        }
    }
}

Dictionary XMLParserWrapper::m_get_attributes(const XMLParser& parser) {
    Dictionary attributes = Dictionary();
    int attributes_count = parser.get_attribute_count();

    for (int attribute_idx = 0; attribute_idx < attributes_count; attribute_idx++) {
        attributes[parser.get_attribute_name(attribute_idx)] = parser.get_attribute_value(attribute_idx);
    }
    return attributes;
}
