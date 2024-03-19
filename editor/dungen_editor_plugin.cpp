#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"

#include "../core/dungen_config.h"

using namespace godot;

void DungenEditorPlugin::_make_visible(bool p_visible) { dungen_editor->set_visible(p_visible); };
void DungenEditorPlugin::_apply_changes(){};
void DungenEditorPlugin::_edit(Object *p_object)
{
    if (Object::cast_to<DungenConfig>(p_object))
    {
        return dungen_editor->_edit_dungen_config(Object::cast_to<DungenConfig>(p_object));
    }
};

bool DungenEditorPlugin::_handles(Object *p_object) const
{
    if (Object::cast_to<DungenConfig>(p_object))
    {
        return true;
    }
    return false;
};

Ref<Texture2D> DungenEditorPlugin::_get_plugin_icon() const
{
    if (!RESOURCE_EXISTS("res://bin/dungen_editor_icon.svg", "Texture2D"))
    {
        UtilityFunctions::print("editor icon not found");
        return nullptr;
    }
    Ref<Texture2D> icon = RESOURCE_LOAD("res://bin/dungen_editor_icon.svg", "Texture2D");
    return icon;
};

DungenEditorPlugin::DungenEditorPlugin()
{
    dungen_editor = memnew(DungenEditor);
    dungen_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
    dungen_editor->set_plugin(this);
    MAIN_SCREEN_CONTROL()->add_child(dungen_editor);
    dungen_editor->hide();
}

DungenEditorPlugin::~DungenEditorPlugin()
{
}

void DungenEditorPlugin::_bind_methods() {}

void DungenEditorPlugin::_notification(int p_what)
{
}
#endif // TOOLS_ENABLED