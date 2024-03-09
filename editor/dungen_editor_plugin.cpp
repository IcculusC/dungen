#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"

#include "../core/dungen_config.h"

using namespace godot;

DungenEditor::DungenEditor()
{
    dungen_instance = memnew(Dungen());

    dungen_image_source = Image::create(128, 128, false, Image::Format::FORMAT_RGBA8);
    dungen_image_texture = ImageTexture::create_from_image(dungen_image_source);

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
    dungen_texture_panel->set_stretch_ratio(3.0);
    hsc->add_child(dungen_texture_panel);

    dungen_texture_rect = memnew(TextureRect);
    dungen_texture_rect->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_texture_rect->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_texture_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
    dungen_texture_panel->add_child(dungen_texture_rect);
    dungen_texture_rect->set_texture(dungen_image_texture);

    side_bar_menu_panel = memnew(PanelContainer);
    side_bar_menu_panel->set_h_size_flags(SIZE_EXPAND_FILL);
    side_bar_menu_panel->set_v_size_flags(SIZE_EXPAND_FILL);
    side_bar_menu_panel->set_stretch_ratio(1.0);
    hsc->add_child(side_bar_menu_panel);
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
    _redraw();
}

void DungenEditor::_redraw()
{
    dungen_image_source->fill(Color(0, 0, 0, 0));

    Array rooms = dungen_instance->get_map();
    Array trimmed_rooms = dungen_instance->get_trimmed_rooms();

    Array all_rooms = dungen_instance->get_all_rooms();

    if (all_rooms.size() == 0)
    {
        return;
    }

    Rect2 bounds = Rect2();

    for (int i = 0; i < all_rooms.size(); i++)
    {
        bounds.expand_to(Ref<DungenRoom>(all_rooms[i])->get_center());
    }

    bounds.grow_by(Math::max(bounds.size.x, bounds.size.y) / 2);

    dungen_image_source->resize(bounds.size.x, bounds.size.y);
    // dungen_image_source->fill(Color::named("GRAY"));

    Vector2 center = bounds.get_size() / 2;

    for (int i = 0; i < all_rooms.size(); i++)
    {
        Ref<DungenRoom> current_room = Ref<DungenRoom>(all_rooms[i]);
        Rect2 rect_copy_hopefully = Rect2(current_room->get_rectangle());
        rect_copy_hopefully.set_position((rect_copy_hopefully.get_position() + center));
        dungen_image_source->fill_rect(rect_copy_hopefully, current_room->get_color());
    }

    dungen_image_source->fill_rect(Rect2(center - Vector2(1, 1), Vector2(2, 2)), Color::named("BLUE"));

    // TODO: investiate - dungen_image_texture->update(dungen_image_source);
    dungen_image_texture->set_image(dungen_image_source); // = ImageTexture::create_from_image(dungen_image_source);
    dungen_texture_rect->set_texture(dungen_image_texture);

    queue_redraw();
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
    }
    if (p_what == NOTIFICATION_THEME_CHANGED)
    {
        new_btn->set_button_icon(get_theme_icon("New", "EditorIcons"));
        load_btn->set_button_icon(get_theme_icon("Load", "EditorIcons"));
        save_btn->set_button_icon(get_theme_icon("Save", "EditorIcons"));
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