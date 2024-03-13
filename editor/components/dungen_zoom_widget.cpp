#ifdef TOOLS_ENABLED

#include "dungen_zoom_widget.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

DungenZoomWidget::DungenZoomWidget()
{
    Ref<StyleBoxEmpty> empty_stylebox = memnew(StyleBoxEmpty);

    zoom_out_btn = memnew(Button);
    zoom_out_btn->set_text("");
    zoom_out_btn->set_tooltip_text("Zoom Out");
    zoom_out_btn->set_flat(true);
    zoom_out_btn->set_focus_mode(Control::FOCUS_NONE);
    add_child(zoom_out_btn);

    zoom_reset_btn = memnew(Button);
    zoom_reset_btn->set_text("");
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
    add_child(zoom_reset_btn);

    zoom_in_btn = memnew(Button);
    zoom_in_btn->set_text("");
    zoom_in_btn->set_tooltip_text("Zoom In");
    zoom_in_btn->set_flat(true);
    zoom_in_btn->set_focus_mode(Control::FOCUS_NONE);
    add_child(zoom_in_btn);
}
DungenZoomWidget::~DungenZoomWidget() {}

void DungenZoomWidget::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("zoom_out_pressed"));
    ADD_SIGNAL(MethodInfo("zoom_reset_pressed"));
    ADD_SIGNAL(MethodInfo("zoom_in_pressed"));
}

void DungenZoomWidget::_emit_pressed(String p_what) {
    emit_signal(p_what);
}

void DungenZoomWidget::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        zoom_out_btn->connect("pressed", callable_mp(this, &DungenZoomWidget::_emit_pressed).bind("zoom_out_pressed"));
        zoom_reset_btn->connect("pressed", callable_mp(this, &DungenZoomWidget::_emit_pressed).bind("zoom_reset_pressed"));
        zoom_in_btn->connect("pressed", callable_mp(this, &DungenZoomWidget::_emit_pressed).bind("zoom_in_pressed"));
    }
    if (p_what == NOTIFICATION_THEME_CHANGED)
    {
        zoom_out_btn->set_button_icon(get_theme_icon("ZoomLess", "EditorIcons"));
        zoom_in_btn->set_button_icon(get_theme_icon("ZoomMore", "EditorIcons"));
    }
}

#endif // TOOLS_ENABLED