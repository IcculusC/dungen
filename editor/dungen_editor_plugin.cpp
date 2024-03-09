#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"

#include "../core/dungen_config.h"

using namespace godot;

DungenEditor::DungenEditor()
{
    set_clip_contents(true);

    dungen_instance = memnew(Dungen());

    save_dialog = memnew(FileDialog);
    save_dialog->set_file_mode(FileDialog::FILE_MODE_SAVE_FILE);
    save_dialog->set_title("Save Dungen Config");
    save_dialog->add_filter("*.tres");
    save_dialog->hide();
    add_child(save_dialog);

    load_dialog = memnew(FileDialog);
    load_dialog->set_file_mode(FileDialog::FILE_MODE_OPEN_FILE);
    load_dialog->set_title("Load Dungen Config");
    load_dialog->add_filter("*.tres");
    load_dialog->hide();
    add_child(load_dialog);

    vbox = memnew(VBoxContainer);
    vbox->set_anchor(SIDE_RIGHT, ANCHOR_END);
    vbox->set_anchor(SIDE_BOTTOM, ANCHOR_END);
    add_child(vbox);

    HBoxContainer *toolbar = memnew(HBoxContainer);
    vbox->add_child(toolbar);

    new_btn = memnew(Button);
    new_btn->set_text("New");
    new_btn->set_tooltip_text("New DungenConfig.");
    new_btn->set_flat(true);
    new_btn->set_focus_mode(Control::FOCUS_NONE);
    toolbar->add_child(new_btn);

    load_btn = memnew(Button);
    load_btn->set_text("Load");
    load_btn->set_tooltip_text("Load DungenConfig.");
    load_btn->set_flat(true);
    load_btn->set_focus_mode(Control::FOCUS_NONE);
    toolbar->add_child(load_btn);

    save_btn = memnew(Button);
    save_btn->set_text("Save");
    save_btn->set_tooltip_text("Save DungenConfig.");
    save_btn->set_flat(true);
    save_btn->set_focus_mode(Control::FOCUS_NONE);
    toolbar->add_child(save_btn);

    header = memnew(Button);
    header->set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);
    header->add_theme_constant_override("hseparation", 8);
    vbox->add_child(header);

    hsc = memnew(HSplitContainer);
    hsc->set_h_size_flags(SIZE_EXPAND_FILL);
    hsc->set_v_size_flags(SIZE_EXPAND_FILL);
    hsc->set_focus_mode(FOCUS_NONE);
    vbox->add_child(hsc);

    dungen_texture_panel = memnew(PanelContainer);
    dungen_texture_panel->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_texture_panel->set_v_size_flags(SIZE_EXPAND_FILL);
//  dungen_texture_panel->set_stretch_ratio(3.0);
    hsc->add_child(dungen_texture_panel);

    side_bar_vbox = memnew(VBoxContainer);
    side_bar_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
    side_bar_vbox->set_v_size_flags(SIZE_EXPAND_FILL);
//  side_bar_menu_panel->set_stretch_ratio(1.0);
    hsc->add_child(side_bar_vbox);

    HBoxContainer *nav = memnew(HBoxContainer);
    nav->set_h_size_flags(SIZE_EXPAND);
    side_bar_vbox->add_child(nav);

    zoom_less_btn = memnew(Button);
	zoom_less_btn->set_text("");
	zoom_less_btn->set_tooltip_text("Zoom Out");
	zoom_less_btn->set_flat(true);
	zoom_less_btn->set_focus_mode(Control::FOCUS_NONE);
	nav->add_child(zoom_less_btn);

    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

    zoom_reset_btn = memnew(Button);
    zoom_reset_btn->set_text("100 %");
    zoom_reset_btn->set_flat(true);
	zoom_reset_btn->set_focus_mode(FOCUS_NONE);
	zoom_reset_btn->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    zoom_reset_btn->add_theme_stylebox_override("normal", empty_stylebox);
	zoom_reset_btn->add_theme_stylebox_override("hover", empty_stylebox);
	zoom_reset_btn->add_theme_stylebox_override("focus", empty_stylebox);
	zoom_reset_btn->add_theme_stylebox_override("pressed", empty_stylebox);
    zoom_reset_btn->add_theme_constant_override("outline_size", 1);
    zoom_reset_btn->add_theme_color_override("font_outline_color", Color(0, 0, 0));
	zoom_reset_btn->add_theme_color_override("font_color", Color(1, 1, 1));
    nav->add_child(zoom_reset_btn);

    zoom_more_btn = memnew(Button);
	zoom_more_btn->set_text("");
	zoom_more_btn->set_tooltip_text("Zoom In");
	zoom_more_btn->set_flat(true);
	zoom_more_btn->set_focus_mode(Control::FOCUS_NONE);
	nav->add_child(zoom_more_btn);

    PanelContainer *preview_wrapper = memnew(PanelContainer);
    preview_wrapper->set_h_size_flags(SIZE_EXPAND_FILL);
    preview_wrapper->set_v_size_flags(SIZE_EXPAND_FILL); 
    preview_wrapper->set_clip_contents(true);
    side_bar_vbox->add_child(preview_wrapper);

    dungen_previewer = memnew(DungenPreviewer); 
    dungen_previewer->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_previewer->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_previewer->set_dungen_instance(dungen_instance);
    preview_wrapper->add_child(dungen_previewer);
}

DungenEditor::~DungenEditor() {}

void DungenEditor::_new_config()
{
    Ref<DungenConfig> config = memnew(DungenConfig);
    EditorInterface::get_singleton()->edit_resource(config);
}

void DungenEditor::_load_config(String p_path)
{
    ERR_FAIL_COND_MSG(p_path.is_empty(), "Empty p_path.");
    Ref<DungenConfig> config = ResourceLoader::get_singleton()->load(p_path, "DungenConfig");
    ERR_FAIL_COND(!config.is_valid());

    EditorInterface::get_singleton()->edit_resource(config);
}

void DungenEditor::_save_config(String p_path)
{
    ERR_FAIL_COND_MSG(p_path.is_empty(), "Empty p_path.");
    ERR_FAIL_COND_MSG(dungen_instance->get_config().is_null(), "DungenConfig is null.");
    ResourceSaver::get_singleton()->save(dungen_instance->get_config(), p_path, ResourceSaver::FLAG_CHANGE_PATH);
}

void DungenEditor::_on_save_pressed()
{
    if (dungen_instance->get_config().is_null())
    {
        return;
    }
    String path = dungen_instance->get_config()->get_path();

    if (path.is_empty())
    {
        // save_dialog->popup_centered_ratio();
        _show_file_dialog(save_dialog);
    }
    else
    {
        _save_config(path);
    }
}

void DungenEditor::_edit_dungen_config(Ref<DungenConfig> config)
{
    ERR_FAIL_COND_MSG(config.is_null(), "config is null.");

    if (!dungen_instance->is_connected("generation_complete", callable_mp(this, &DungenEditor::_generation_complete)))
    {
        dungen_instance->connect("generation_complete", callable_mp(this, &DungenEditor::_generation_complete));
    }

    config->connect("changed", callable_mp(this, &DungenEditor::_regenerate));

    dungen_instance->set_config(config);

    _regenerate();
}

void DungenEditor::_regenerate()
{
    dungen_instance->generate();
}

void DungenEditor::_generation_complete(double p_time)
{
    dungen_previewer->queue_redraw();
}

void DungenEditor::_update_zoom_reset_button() {
    String zoom_factor = rtos((dungen_previewer->get_scale().x / 1.0) * 100);
    zoom_reset_btn->set_text(zoom_factor + " %");
}

void DungenEditor::_reset_zoom() {
    dungen_previewer->set_scale(Vector2(1, 1)); 
    _update_zoom_reset_button();
}

void DungenEditor::_zoom_editor(Vector2 amount) {
    Vector2 zoom = (dungen_previewer->get_scale() + amount).clamp(Vector2(1, 1), Vector2(10, 10));
    dungen_previewer->set_scale(zoom);
    _update_zoom_reset_button();
}

void DungenEditor::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        save_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_save_config));
        load_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_load_config));
        new_btn->connect("pressed", callable_mp(this, &DungenEditor::_new_config));
        load_btn->connect("pressed", callable_mp(this, &DungenEditor::_show_file_dialog).bind(load_dialog));
        save_btn->connect("pressed", callable_mp(this, &DungenEditor::_on_save_pressed));

        zoom_less_btn->connect("pressed", callable_mp(this, &DungenEditor::_zoom_editor).bind(Vector2(-0.5, -0.5)));
        zoom_reset_btn->connect("pressed", callable_mp(this, &DungenEditor::_reset_zoom));
        zoom_more_btn->connect("pressed", callable_mp(this, &DungenEditor::_zoom_editor).bind(Vector2(0.5, 0.5)));
    }
    if (p_what == NOTIFICATION_THEME_CHANGED)
    {
        new_btn->set_button_icon(get_theme_icon("New", "EditorIcons"));
        load_btn->set_button_icon(get_theme_icon("Load", "EditorIcons"));
        save_btn->set_button_icon(get_theme_icon("Save", "EditorIcons"));

        zoom_less_btn->set_button_icon(get_theme_icon("ZoomLess", "EditorIcons"));
        zoom_more_btn->set_button_icon(get_theme_icon("ZoomMore", "EditorIcons"));
    }
}

void DungenEditor::_bind_methods() {}

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
    if (!ResourceLoader::get_singleton()->exists("res://dungen/dungen_editor_icon.svg", "Texture2D"))
    {
        UtilityFunctions::print("editor icon not found");
        return nullptr;
    }
    Ref<Texture2D> icon = ResourceLoader::get_singleton()->load("res://dungen/dungen_editor_icon.svg", "Texture2D");
    return icon;
};

DungenEditorPlugin::DungenEditorPlugin() {}

DungenEditorPlugin::~DungenEditorPlugin() {}

void DungenEditorPlugin::_bind_methods() {}

void DungenEditorPlugin::_notification(int p_what)
{
    if (p_what == NOTIFICATION_ENTER_TREE)
    {
        dungen_editor = memnew(DungenEditor());
        dungen_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
        EditorInterface::get_singleton()->get_editor_main_screen()->add_child(dungen_editor);
        dungen_editor->hide();
        dungen_editor->set_plugin(this);
    }
    else if (p_what == NOTIFICATION_EXIT_TREE)
    {
        memdelete(dungen_editor);
        // dungen_editor = nullptr;
    }
}
#endif // TOOLS_ENABLED