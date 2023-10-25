#ifndef XML_PARSER_WRAPPER_HPP
#define XML_PARSER_WRAPPER_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/xml_parser.hpp>

namespace godot {
    class XMLParserWrapper : public RefCounted {
        GDCLASS(XMLParserWrapper, RefCounted)

    public:
        XMLParserWrapper();
        ~XMLParserWrapper() override;

        static XMLParserWrapper *get_singleton();

        Dictionary parse_file(const String& path);
        Dictionary parse_string(const String& xml);
        Dictionary parse_buffer(const PackedByteArray& xml);

    protected:
        static void _bind_methods();

    private:
        static XMLParserWrapper *xmlParserWrapper;

        static Dictionary _parse(const PackedByteArray& xml);
        static Variant _make_node(const Ref<XMLParser>& parser, Array &queue);
        static Dictionary _get_attributes(const Ref<XMLParser>& parser);

        Dictionary _to_dict(Dictionary node, bool include_empty_fields = false, const String& node_content_field_name = "__content__");
        static Dictionary _to_dict_all_fields(Dictionary node);
        static Dictionary _to_dict_least_fields(Dictionary node);
    };
}

#endif //XML_PARSER_WRAPPER_HPP
