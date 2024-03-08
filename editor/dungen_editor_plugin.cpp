#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"

using namespace godot;

DungenEditor::DungenEditor() {}

DungenEditor::~DungenEditor() {}

void DungenEditor::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
    }
    if (p_what == NOTIFICATION_THEME_CHANGED) {
    }
}

void DungenEditor::_bind_methods() {}

void DungenEditorPlugin::_make_visible(bool p_visible) { dungen_editor->set_visible(p_visible); };
void DungenEditorPlugin::_apply_changes() {};
void DungenEditorPlugin::_edit(Object *p_object) {};

bool DungenEditorPlugin::_handles(Object *p_object) const { return false; };

Ref<Texture2D> DungenEditorPlugin::_get_plugin_icon() const {
    if (!ResourceLoader::get_singleton()->exists("res://godot-dungen/dungen_editor_icon.svg", "Texture2D")) {
        UtilityFunctions::print("editor icon not found");
        return nullptr;
    }
    Ref<Texture2D> icon = ResourceLoader::get_singleton()->load("res://godot-dungen/dungen_editor_icon.svg", "Texture2D");
    return icon;
};

DungenEditorPlugin::DungenEditorPlugin() {}

DungenEditorPlugin::~DungenEditorPlugin() {}

void DungenEditorPlugin::_bind_methods() {}

void DungenEditorPlugin::_notification(int p_what) {
    if (p_what == NOTIFICATION_ENTER_TREE) {
        dungen_editor = memnew(DungenEditor());
        dungen_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
        EditorInterface::get_singleton()->get_editor_main_screen()->add_child(dungen_editor);
        dungen_editor->hide();
        dungen_editor->set_plugin(this);
    } else if (p_what == NOTIFICATION_EXIT_TREE) {
        memdelete(dungen_editor);
        // dungen_editor = nullptr;
    }
}
#endif // TOOLS_ENABLED