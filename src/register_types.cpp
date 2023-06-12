#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include "ogg_loader.hpp"
// #include "ogg_loader_tree.hpp"
// #include "my_singleton.hpp"

using namespace godot;

// static MySingleton *_my_singleton;
static OggLoader *_ogg_loader;
// static OggLoaderTree *_ogg_loader_tree;

void gdextension_initialize(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<OggLoader>();
//		ClassDB::register_class<OggLoaderTree>();
//		ClassDB::register_class<MySingleton>();

		_ogg_loader = memnew(OggLoader);
		Engine::get_singleton()->register_singleton("OggLoader", OggLoader::get_singleton());
//		_ogg_loader_tree = memnew(OggLoaderTree);
//		Engine::get_singleton()->register_singleton("OggLoaderTree", OggLoaderTree::get_singleton());
//		_my_singleton = memnew(MySingleton);
//		Engine::get_singleton()->register_singleton("MySingleton", MySingleton::get_singleton());
	}
}

void gdextension_terminate(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		Engine::get_singleton()->unregister_singleton("OggLoader");
		memdelete(_ogg_loader);
//		Engine::get_singleton()->unregister_singleton("OggLoaderTree");
//		memdelete(_ogg_loader_tree);
//		Engine::get_singleton()->unregister_singleton("MySingleton");
//		memdelete(_my_singleton);
	}
}

extern "C"
{
	GDExtensionBool GDE_EXPORT gdextension_init(const GDExtensionInterface *p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

		init_obj.register_initializer(gdextension_initialize);
		init_obj.register_terminator(gdextension_terminate);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
