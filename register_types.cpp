#include "register_types.h"

#include "core/dungen_config.h"
#include "core/dungen.h"

#ifdef TOOLS_ENABLED
#include "editor/dungen_editor_plugin.h"
#include "editor/components/dungen_editor_toolbar.h"
#include "editor/components/dungen_zoom_widget.h"
#include "editor/dungen_preview_panel.h"
#include "editor/dungen_previewer.h"
#endif // TOOLS_ENABLED

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_dungen_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(DungenConfig);
		GDREGISTER_CLASS(Dungen);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		UtilityFunctions::print("EDITOR INITIALIZED");

		GDREGISTER_CLASS(DungenPreviewer)
		GDREGISTER_CLASS(DungenZoomWidget)
		GDREGISTER_CLASS(DungenPreviewPanel)
		GDREGISTER_CLASS(DungenEditorToolbar)
		GDREGISTER_CLASS(DungenEditor)
		GDREGISTER_CLASS(DungenEditorPlugin)

		EditorPlugins::add_by_type<DungenEditorPlugin>();
	}
#endif // TOOLS_ENABLED
}

void uninitialize_dungen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT dungen_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_dungen_module);
	init_obj.register_terminator(uninitialize_dungen_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
