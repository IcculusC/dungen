#ifdef TOOLS_ENABLED

#include "dungen_editor_toolbar.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

DungenEditorToolbar::DungenEditorToolbar()
{
    new_btn = memnew(Button);
    new_btn->set_text("New");
    new_btn->set_tooltip_text("New DungenConfig.");
    new_btn->set_flat(true);
    new_btn->set_focus_mode(Control::FOCUS_NONE);
    add_child(new_btn);

    load_btn = memnew(Button);
    load_btn->set_text("Load");
    load_btn->set_tooltip_text("Load DungenConfig.");
    load_btn->set_flat(true);
    load_btn->set_focus_mode(Control::FOCUS_NONE);
    add_child(load_btn);

    save_btn = memnew(Button);
    save_btn->set_text("Save");
    save_btn->set_tooltip_text("Save DungenConfig.");
    save_btn->set_flat(true);
    save_btn->set_focus_mode(Control::FOCUS_NONE);
    add_child(save_btn);
}
DungenEditorToolbar::~DungenEditorToolbar() {}

void DungenEditorToolbar::_emit_pressed(String p_which) {
    emit_signal(p_which);
}

void DungenEditorToolbar::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        new_btn->connect("pressed", callable_mp(this, &DungenEditorToolbar::_emit_pressed).bind("new_pressed"));
        load_btn->connect("pressed", callable_mp(this, &DungenEditorToolbar::_emit_pressed).bind("load_pressed"));
        save_btn->connect("pressed", callable_mp(this, &DungenEditorToolbar::_emit_pressed).bind("save_pressed"));
    }
    if (p_what == NOTIFICATION_THEME_CHANGED)
    {
        new_btn->set_button_icon(get_theme_icon("New", "EditorIcons"));
        load_btn->set_button_icon(get_theme_icon("Load", "EditorIcons"));
        save_btn->set_button_icon(get_theme_icon("Save", "EditorIcons"));
    }
}

void DungenEditorToolbar::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("new_pressed"));
    ADD_SIGNAL(MethodInfo("load_pressed"));
    ADD_SIGNAL(MethodInfo("save_pressed"));
}

#endif // TOOLS_ENABLED