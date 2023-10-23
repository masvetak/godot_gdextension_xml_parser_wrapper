#ifndef XML_DOCUMENT_HPP
#define XML_DOCUMENT_HPP

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {
    class XMLDocument : public RefCounted {
    private:
        static Dictionary m_to_dict(Dictionary node, bool include_empty_fields = false, const String& node_content_field_name = "__content__");
        static Dictionary m_to_dict_all_fields(Dictionary node);
        static Dictionary m_to_dict_least_fields(Dictionary node);

    public:
        XMLDocument();
        ~XMLDocument() override;

        Dictionary root;
        Dictionary to_dict(bool include_empty_fields = false, const String& node_content_field_name = "__content__") const;
    };
}

#endif //XML_DOCUMENT_HPP
