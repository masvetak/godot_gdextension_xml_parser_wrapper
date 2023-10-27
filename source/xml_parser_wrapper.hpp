#ifndef XML_PARSER_WRAPPER_HPP
#define XML_PARSER_WRAPPER_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/xml_parser.hpp>
#include <vector>

using namespace std;

namespace godot {
    class XMLParserWrapper : public RefCounted {
        GDCLASS(XMLParserWrapper, RefCounted)

        typedef struct xml_node_t {
            String name = "";
            Dictionary attributes = Dictionary();
            String content = "";
            bool standalone = false;
            vector<xml_node_t> children = vector<xml_node_t>();
        } xml_node_t;

    public:
        XMLParserWrapper();
        ~XMLParserWrapper() override;

        static XMLParserWrapper *get_singleton();

        Dictionary parse_file(const String& path);
        Dictionary parse_string(const String& xml);

    protected:
        static void _bind_methods();

    private:
        static XMLParserWrapper *xmlParserWrapper;

        Dictionary _parse(const PackedByteArray& xml);
        static Dictionary _get_attributes(const Ref<XMLParser>& parser);

        Dictionary _to_dict(const xml_node_t& node, bool include_empty_fields = false, const String& node_content_field_name = "__content__");
        static Dictionary _to_dict_all_fields(const xml_node_t& node);
        static Dictionary _to_dict_least_fields(const xml_node_t& node);
    };
}

#endif //XML_PARSER_WRAPPER_HPP
