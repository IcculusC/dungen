#ifdef TOOLS_ENABLED

#include "dungen_editor_plugin.h"
#include "dungen_editor_macros.h"

#include "../core/dungen_config.h"

using namespace godot;

void DungenEditor::_initialize_dialogs()
{
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
}

void DungenEditor::_initialize_main_layout()
{
    main_layout = memnew(VBoxContainer);
    main_layout->set_anchor(SIDE_RIGHT, ANCHOR_END);
    main_layout->set_anchor(SIDE_BOTTOM, ANCHOR_END);
    add_child(main_layout);

    dungen_editor_toolbar = memnew(DungenEditorToolbar);
    main_layout->add_child(dungen_editor_toolbar);

    header = memnew(Button);
    header->set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);
    header->add_theme_constant_override("hseparation", 8);
    main_layout->add_child(header);

    MarginContainer *main_margins = memnew(MarginContainer);
    EXPAND_FILL(main_margins)
    MARGIN_OVERRIDE_ALL(main_margins, 8)
    MARGIN_OVERRIDE(main_margins, "top", 0)
    main_layout->add_child(main_margins);

    hsc = memnew(HSplitContainer);
    EXPAND_FILL(hsc)
    hsc->set_focus_mode(FOCUS_NONE);
    main_margins->add_child(hsc);
}

DungenEditor::DungenEditor()
{
    set_clip_contents(true);
    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

    animation_timer = memnew(Timer);
    add_child(animation_timer);
    animation_timer->connect("timeout", callable_mp(this, &DungenEditor::_step));
    
    dungen_instance = memnew(Dungen());
    add_child(dungen_instance);

    _initialize_dialogs();
    _initialize_main_layout();

    dungen_preview_panel = memnew(DungenPreviewPanel);
    EXPAND_FILL(dungen_preview_panel)
    dungen_preview_panel->set_stretch_ratio(4);
    dungen_preview_panel->set_dungen_instance(dungen_instance);
    hsc->add_child(dungen_preview_panel);

    Ref<StyleBoxFlat> panel_bg = EDITOR_THEME()->get_stylebox("panel", "Panel");

    dungen_preview_sidebar_panel = memnew(PanelContainer);
    ADD_STYLEBOX_OVERRIDE(dungen_preview_sidebar_panel, "panel", panel_bg)
    EXPAND_FILL(dungen_preview_sidebar_panel)
    dungen_preview_sidebar_panel->set_stretch_ratio(1);
    hsc->add_child(dungen_preview_sidebar_panel);

    dungen_preview_sidebar_layout = memnew(VBoxContainer);
    EXPAND_FILL(dungen_preview_sidebar_layout)
    dungen_preview_sidebar_panel->add_child(dungen_preview_sidebar_layout);

    show_generation_animation = false;
    show_generation_animation_btn = memnew(CheckButton);
    show_generation_animation_btn->set_text("Animate Generation");
    show_generation_animation_btn->set_toggle_mode(true);
    show_generation_animation_btn->set_pressed(show_generation_animation);
    dungen_preview_sidebar_layout->add_child(show_generation_animation_btn);

    show_path_rectangles_btn = memnew(CheckButton);
    show_path_rectangles_btn->set_text("Path Rectangles");
    show_path_rectangles_btn->set_toggle_mode(true);
    show_path_rectangles_btn->set_pressed(true);
    dungen_preview_panel->set_show_path_rectangles(true);
    dungen_preview_sidebar_layout->add_child(show_path_rectangles_btn);

    show_trimmed_rooms_btn = memnew(CheckButton);
    show_trimmed_rooms_btn->set_text("Trimmed Rooms");
    show_trimmed_rooms_btn->set_toggle_mode(true);
    show_trimmed_rooms_btn->set_pressed(false);
    dungen_preview_panel->set_show_trimmed_rooms(false);
    dungen_preview_sidebar_layout->add_child(show_trimmed_rooms_btn);

    show_triangulation_btn = memnew(CheckButton);
    show_triangulation_btn->set_text("Triangulation");
    show_triangulation_btn->set_toggle_mode(true);
    show_triangulation_btn->set_pressed(false);
    dungen_preview_panel->set_show_triangulation(false);
    dungen_preview_sidebar_layout->add_child(show_triangulation_btn);

    show_minimum_spanning_tree_btn = memnew(CheckButton);
    show_minimum_spanning_tree_btn->set_text("Minimum Spanning Tree");
    show_minimum_spanning_tree_btn->set_toggle_mode(true);
    show_minimum_spanning_tree_btn->set_pressed(false);
    dungen_preview_panel->set_show_minimum_spanning_tree(false);
    dungen_preview_sidebar_layout->add_child(show_minimum_spanning_tree_btn);

    show_path_edges_btn = memnew(CheckButton);
    show_path_edges_btn->set_text("Path Edges");
    show_path_edges_btn->set_toggle_mode(true);
    show_path_edges_btn->set_pressed(false);
    dungen_preview_panel->set_show_path_edges(false);
    dungen_preview_sidebar_layout->add_child(show_path_edges_btn);
}

DungenEditor::~DungenEditor()
{
}

void DungenEditor::_new_config()
{
    Ref<DungenConfig> config = memnew(DungenConfig);
    EDITOR_INTERFACE()->edit_resource(config);
}

void DungenEditor::_load_config(String p_path)
{
    ERR_FAIL_COND_MSG(p_path.is_empty(), "Empty p_path.");
    Ref<DungenConfig> config = RESOURCE_LOAD(p_path, "DungenConfig");
    ERR_FAIL_COND(!config.is_valid());

    EDITOR_INTERFACE()->edit_resource(config);
}

void DungenEditor::_save_config(String p_path)
{
    ERR_FAIL_COND_MSG(p_path.is_empty(), "Empty p_path.");
    ERR_FAIL_COND_MSG(dungen_instance->get_config().is_null(), "DungenConfig is null.");
    RESOURCE_SAVE(dungen_instance->get_config(), p_path, ResourceSaver::FLAG_CHANGE_PATH);
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

void DungenEditor::_set_show_generation_animation(bool p_show)
{
    if (show_generation_animation == p_show)
    {
        return;
    }
    show_generation_animation = p_show;

    _regenerate();
}

void DungenEditor::_regenerate()
{

    if (show_generation_animation)
    {
        animation_timer->stop();
        dungen_instance->begin();
        animation_timer->start(0.05);
    }
    else
    {
        animation_timer->stop();
        dungen_instance->generate();
    }
}

void DungenEditor::_step()
{
    if (dungen_instance->next() != -1)
    {
        switch (dungen_instance->get_phase())
        {
        case 0:
        case 1:
            dungen_preview_panel->set_show_all(true);
            break;
        case 2:
            dungen_preview_panel->set_show_all(false);
            break;
        case 3:
        default:
        {
            animation_timer->stop();
        }
        }
    }

    dungen_preview_panel->refresh();
    /*
    if (animation_iterator != dungen_instance->end()) {
        if (animation_iterator.get_stage() > 1) {
            dungen_preview_panel->set_show_all(false);
        } else {
            dungen_preview_panel->set_show_all(true);
        }
        animation_iterator.next();

        dungen_preview_panel->refresh();

        Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.1);
        timer->connect("timeout", callable_mp(this, &DungenEditor::_step));
    }
    */
}

void DungenEditor::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        save_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_save_config));
        load_dialog->connect("file_selected", callable_mp(this, &DungenEditor::_load_config));

        show_generation_animation_btn->connect("toggled", callable_mp(this, &DungenEditor::_set_show_generation_animation));
        show_path_rectangles_btn->connect("toggled", callable_mp(dungen_preview_panel, &DungenPreviewPanel::set_show_path_rectangles));
        show_trimmed_rooms_btn->connect("toggled", callable_mp(dungen_preview_panel, &DungenPreviewPanel::set_show_trimmed_rooms));
        show_triangulation_btn->connect("toggled", callable_mp(dungen_preview_panel, &DungenPreviewPanel::set_show_triangulation));
        show_minimum_spanning_tree_btn->connect("toggled", callable_mp(dungen_preview_panel, &DungenPreviewPanel::set_show_minimum_spanning_tree));
        show_path_edges_btn->connect("toggled", callable_mp(dungen_preview_panel, &DungenPreviewPanel::set_show_path_edges));

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
    if (!RESOURCE_EXISTS("res://dungen/dungen_editor_icon.svg", "Texture2D"))
    {
        UtilityFunctions::print("editor icon not found");
        return nullptr;
    }
    Ref<Texture2D> icon = RESOURCE_LOAD("res://dungen/dungen_editor_icon.svg", "Texture2D");
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
        MAIN_SCREEN_CONTROL()->add_child(dungen_editor);
        dungen_editor->hide();
        dungen_editor->set_plugin(this);
    }
    else if (p_what == NOTIFICATION_EXIT_TREE)
    {
    }
}
#endif // TOOLS_ENABLED