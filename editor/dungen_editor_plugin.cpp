#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"

#include "../core/dungen_config.h"

using namespace godot;

DungenEditor::DungenEditor()
{
    set_clip_contents(true);

    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

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


    dungen_editor_toolbar = memnew(DungenEditorToolbar);
    vbox->add_child(dungen_editor_toolbar);


    header = memnew(Button);
    header->set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);
    header->add_theme_constant_override("hseparation", 8);
    vbox->add_child(header);


    hsc = memnew(HSplitContainer);
    hsc->set_h_size_flags(SIZE_EXPAND_FILL);
    hsc->set_v_size_flags(SIZE_EXPAND_FILL);
    hsc->set_dragger_visibility(HSplitContainer::DRAGGER_HIDDEN);
    hsc->set_focus_mode(FOCUS_NONE);
    vbox->add_child(hsc);

    dungen_preview_panel = memnew(DungenPreviewPanel);
    dungen_preview_panel->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_panel->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_panel->set_stretch_ratio(4);
    dungen_preview_panel->set_dungen_instance(dungen_instance);
    hsc->add_child(dungen_preview_panel);

    PanelContainer *dungen_preview_sidebar_panel = memnew(PanelContainer);
    dungen_preview_sidebar_panel->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_sidebar_panel->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_sidebar_panel->set_stretch_ratio(1);
    hsc->add_child(dungen_preview_sidebar_panel);

    dungen_preview_sidebar_vbox = memnew(VBoxContainer);
    dungen_preview_sidebar_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_sidebar_vbox->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_sidebar_panel->add_child(dungen_preview_sidebar_vbox); 

    show_trimmed_rooms_btn = memnew(CheckButton);
    show_trimmed_rooms_btn->set_text("Show Trimmed Rooms");
    show_trimmed_rooms_btn->set_toggle_mode(true);
    dungen_preview_sidebar_vbox->add_child(show_trimmed_rooms_btn);
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

    config->connect("changed", callable_mp(this, &DungenEditor::_regenerate));

    dungen_instance->set_config(config);

    _regenerate();
}

void DungenEditor::_regenerate()
{
    dungen_instance->generate();
}

void DungenEditor::_show_trimmed_rooms(bool p_show) {
    dungen_preview_panel->set_show_trimmed_rooms(p_show);
}

void DungenEditor::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        save_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_save_config));
        load_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_load_config));

        show_trimmed_rooms_btn->connect("toggled", callable_mp(this, &DungenEditor::_show_trimmed_rooms));
    
        dungen_editor_toolbar->connect("new_pressed", callable_mp(this, &DungenEditor::_new_config));
        dungen_editor_toolbar->connect("load_pressed", callable_mp(this, &DungenEditor::_show_file_dialog).bind(load_dialog));
        dungen_editor_toolbar->connect("save_pressed", callable_mp(this, &DungenEditor::_on_save_pressed));
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