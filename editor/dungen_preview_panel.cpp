#ifdef TOOLS_ENABLED

#include "dungen_preview_panel.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

DungenPreviewPanel::DungenPreviewPanel()
{
    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

    vbox = memnew(VBoxContainer);
    vbox->set_h_size_flags(SIZE_EXPAND_FILL);
    vbox->set_v_size_flags(SIZE_EXPAND_FILL);
    add_child(vbox);

    nav = memnew(HBoxContainer);
    nav->set_h_size_flags(SIZE_EXPAND);
    vbox->add_child(nav);

    zoom_out_btn = memnew(Button);
    zoom_out_btn->set_text("");
    zoom_out_btn->set_tooltip_text("Zoom Out");
    zoom_out_btn->set_flat(true);
    zoom_out_btn->set_focus_mode(Control::FOCUS_NONE);
    nav->add_child(zoom_out_btn);

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

    zoom_in_btn = memnew(Button);
    zoom_in_btn->set_text("");
    zoom_in_btn->set_tooltip_text("Zoom In");
    zoom_in_btn->set_flat(true);
    zoom_in_btn->set_focus_mode(Control::FOCUS_NONE);
    nav->add_child(zoom_in_btn);

    dungen_preview_wrapper = memnew(PanelContainer);
    dungen_preview_wrapper->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->set_clip_contents(true);
    vbox->add_child(dungen_preview_wrapper);

    dungen_previewer = memnew(DungenPreviewer);
    dungen_previewer->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_previewer->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->add_child(dungen_previewer);
}
DungenPreviewPanel::~DungenPreviewPanel() {}

void DungenPreviewPanel::set_dungen_instance(Dungen *dungen)
{
    dungen_previewer->set_dungen_instance(dungen);
}

void DungenPreviewPanel::set_show_trimmed_rooms(bool s_show)
{
    dungen_previewer->set_show_trimmed_rooms(s_show);
}

void DungenPreviewPanel::set_show_triangulation(bool s_show)
{
    dungen_previewer->set_show_triangulation(s_show);
}

void DungenPreviewPanel::set_show_minimum_spanning_tree(bool s_show)
{
    dungen_previewer->set_show_minimum_spanning_tree(s_show);
}

void DungenPreviewPanel::set_show_path_edges(bool s_show)
{
    dungen_previewer->set_show_path_edges(s_show);
}

void DungenPreviewPanel::refresh()
{
    dungen_previewer->queue_redraw();
}

void DungenPreviewPanel::_update_zoom_reset_button()
{
    String zoom_factor = rtos((dungen_previewer->get_scale().x / 1.0) * 100);
    zoom_reset_btn->set_text(zoom_factor + " %");
}

void DungenPreviewPanel::_reset_previewer_zoom()
{
    dungen_previewer->set_scale(Vector2(1, 1));
    _update_zoom_reset_button();
}

void DungenPreviewPanel::_update_previewer_zoom(Vector2 amount)
{
    Vector2 zoom = (dungen_previewer->get_scale() + amount).clamp(Vector2(1, 1), Vector2(10, 10));
    dungen_previewer->set_scale(zoom);
    _update_zoom_reset_button();
}

void DungenPreviewPanel::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        zoom_out_btn->connect("pressed", callable_mp(this, &DungenPreviewPanel::_update_previewer_zoom).bind(Vector2(-0.25, -0.25)));
        zoom_reset_btn->connect("pressed", callable_mp(this, &DungenPreviewPanel::_reset_previewer_zoom));
        zoom_in_btn->connect("pressed", callable_mp(this, &DungenPreviewPanel::_update_previewer_zoom).bind(Vector2(0.25, 0.25)));
    }
    if (p_what == NOTIFICATION_THEME_CHANGED)
    {
        zoom_out_btn->set_button_icon(get_theme_icon("ZoomLess", "EditorIcons"));
        zoom_in_btn->set_button_icon(get_theme_icon("ZoomMore", "EditorIcons"));
    }
    if (p_what == NOTIFICATION_RESIZED)
    {
        _reset_previewer_zoom();
    }
}

void DungenPreviewPanel::_bind_methods()
{
}

#endif // TOOLS_ENABLED