#ifdef TOOLS_ENABLED

#include "dungen_preview_panel.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

DungenPreviewPanel::DungenPreviewPanel()
{
    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

    main_toolbar = memnew(HBoxContainer);
    add_child(main_toolbar);

    dungen_zoom_widget = memnew(DungenZoomWidget);
    main_toolbar->add_child(dungen_zoom_widget);

    dungen_grid_widget = memnew(HBoxContainer);
    main_toolbar->add_child(dungen_grid_widget);

    dungen_grid_widget_label = memnew(Label);
    dungen_grid_widget_label->set_text("Grid Size");
    main_toolbar->add_child(dungen_grid_widget_label);

    dungen_grid_widget_spin_box = memnew(SpinBox);
    dungen_grid_widget_spin_box->set_min(8.0);
    dungen_grid_widget_spin_box->set_max(64.0);
    dungen_grid_widget_spin_box->set_step(8.0);
    dungen_grid_widget_spin_box->set_custom_arrow_step(8.0);
    main_toolbar->add_child(dungen_grid_widget_spin_box);

    dungen_grid_show_checkbox = memnew(CheckBox);
    dungen_grid_show_checkbox->set_text("Show Grid");
    dungen_grid_show_checkbox->set_pressed(true);
    main_toolbar->add_child(dungen_grid_show_checkbox);

    dungen_preview_wrapper = memnew(PanelContainer);
    dungen_preview_wrapper->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->set_clip_contents(true);
    add_child(dungen_preview_wrapper);

    dungen_previewer = memnew(DungenPreviewer);
    dungen_previewer->set_h_size_flags(SIZE_EXPAND_FILL);
    dungen_previewer->set_v_size_flags(SIZE_EXPAND_FILL);
    dungen_preview_wrapper->add_child(dungen_previewer);
}
DungenPreviewPanel::~DungenPreviewPanel() {}

void DungenPreviewPanel::set_dungen_instance(Dungen *dungen)
{
    ERR_FAIL_COND_MSG(!dungen, "Provided Dungen is null.");

    dungen_previewer->set_dungen_instance(dungen);
}

void DungenPreviewPanel::set_show_grid(bool p_show)
{
    dungen_previewer->set_show_grid(p_show);
}

void DungenPreviewPanel::set_show_all(bool p_show)
{
    dungen_previewer->set_show_all(p_show);
}

void DungenPreviewPanel::set_show_trimmed_rooms(bool p_show)
{
    dungen_previewer->set_show_trimmed_rooms(p_show);
}

void DungenPreviewPanel::set_show_triangulation(bool p_show)
{
    dungen_previewer->set_show_triangulation(p_show);
}

void DungenPreviewPanel::set_show_minimum_spanning_tree(bool p_show)
{
    dungen_previewer->set_show_minimum_spanning_tree(p_show);
}

void DungenPreviewPanel::set_show_path_edges(bool p_show)
{
    dungen_previewer->set_show_path_edges(p_show);
}

void DungenPreviewPanel::set_show_path_rectangles(bool p_show)
{
    dungen_previewer->set_show_path_rectangles(p_show);
}

void DungenPreviewPanel::refresh()
{
    dungen_previewer->refresh();
}

void DungenPreviewPanel::_update_zoom_reset_button()
{
    String zoom_factor = rtos((dungen_previewer->get_zoom().x / 1.0) * 100);
    dungen_zoom_widget->set_text(zoom_factor + " %");
}

void DungenPreviewPanel::_reset_previewer_zoom()
{
    dungen_previewer->set_zoom(Vector2(1, 1));
}

void DungenPreviewPanel::_update_previewer_zoom(const Vector2 &p_amount)
{
    Vector2 zoom = dungen_previewer->get_zoom() + p_amount;
    dungen_previewer->set_zoom(zoom);
}

void DungenPreviewPanel::_update_grid_size(const double &p_size)
{
    dungen_previewer->set_grid_size(p_size);
}

void DungenPreviewPanel::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        dungen_zoom_widget->connect("zoom_out_pressed", callable_mp(this, &DungenPreviewPanel::_update_previewer_zoom).bind(Vector2(-0.25, -0.25)));
        dungen_zoom_widget->connect("zoom_reset_pressed", callable_mp(this, &DungenPreviewPanel::_reset_previewer_zoom));
        dungen_zoom_widget->connect("zoom_in_pressed", callable_mp(this, &DungenPreviewPanel::_update_previewer_zoom).bind(Vector2(0.25, 0.25)));
        dungen_previewer->connect("zoom_changed", callable_mp(this, &DungenPreviewPanel::_update_zoom_reset_button));
        dungen_grid_widget_spin_box->connect("value_changed", callable_mp(this, &DungenPreviewPanel::_update_grid_size));
        dungen_grid_show_checkbox->connect("toggled", callable_mp(this, &DungenPreviewPanel::set_show_grid));
        _update_previewer_zoom(Vector2(3, 3));
    }
}

void DungenPreviewPanel::_bind_methods()
{
}

#endif // TOOLS_ENABLED