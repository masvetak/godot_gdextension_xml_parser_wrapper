#ifndef XML_PARSER_WRAPPER_HPP
#define XML_PARSER_WRAPPER_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/xml_parser.hpp>

#include "xml_document.hpp"

namespace godot {
    class XMLParserWrapper : public RefCounted {
        GDCLASS(XMLParserWrapper, RefCounted)

    private:
        static XMLParserWrapper *xmlParserWrapper;

        static XMLDocument m_parse(const PackedByteArray& xml);
        static Variant m_make_node(Array &queue, XMLParser parser);
        static Dictionary m_get_attributes(const XMLParser& parser);

    protected:
        static void _bind_methods();

    public:
        XMLParserWrapper();
        ~XMLParserWrapper() override;

        static XMLParserWrapper *get_singleton();

        Dictionary parse_file(const String& path);
        Dictionary parse_string(const String& xml);
        Dictionary parse_buffer(const PackedByteArray& xml);
    };
}

#endif //XML_PARSER_WRAPPER_HPP
