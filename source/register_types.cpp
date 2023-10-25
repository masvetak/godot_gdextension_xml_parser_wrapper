#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include "xml_parser_wrapper.hpp"

using namespace godot;

static XMLParserWrapper *xmlParserWrapper;

void initialize_xml_parser_wrapper(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<XMLParserWrapper>();

    xmlParserWrapper = memnew(XMLParserWrapper);
    Engine::get_singleton()->register_singleton("XMLParserWrapper", XMLParserWrapper::get_singleton());
}

void uninitialize_xml_parser_wrapper(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    Engine::get_singleton()->unregister_singleton("XMLParserWrapper");
    memdelete(xmlParserWrapper);
}

extern "C" {
    GDExtensionBool GDE_EXPORT xml_parser_wrapper_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_xml_parser_wrapper);
        init_obj.register_terminator(uninitialize_xml_parser_wrapper);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
