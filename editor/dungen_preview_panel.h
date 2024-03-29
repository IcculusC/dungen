#ifdef TOOLS_ENABLED

#ifndef DUNGEN_PREVIEW_PANEL_H
#define DUNGEN_PREVIEW_PANEL_H

#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/check_box.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/spin_box.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <godot_cpp/variant/string.hpp>

#include "../core/dungen.h"
#include "dungen_previewer.h"
#include "components/dungen_zoom_widget.h"

namespace godot
{
    class DungenPreviewPanel : public VBoxContainer
    {
        GDCLASS(DungenPreviewPanel, VBoxContainer)

    private:
        HBoxContainer *main_toolbar;

        DungenZoomWidget *dungen_zoom_widget;

        HBoxContainer *dungen_grid_widget;
        Label *dungen_grid_widget_label;
        SpinBox *dungen_grid_widget_spin_box;
        CheckBox *dungen_grid_show_checkbox;

        PanelContainer *dungen_preview_wrapper;

        DungenPreviewer *dungen_previewer;

        void _reset_previewer_zoom();
        void _update_zoom_reset_button();
        void _update_previewer_zoom(const Vector2 &p_amount);
        void _update_grid_size(const double &p_size);

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        DungenPreviewPanel();
        ~DungenPreviewPanel();

        void set_dungen_instance(Dungen *dungen);

        void set_show_grid(bool p_show);

        void set_show_all(bool p_show);
        void set_show_trimmed_rooms(bool p_show);
        void set_show_triangulation(bool p_show);
        void set_show_minimum_spanning_tree(bool p_show);
        void set_show_path_edges(bool p_show);
        void set_show_path_rectangles(bool p_show);

        void refresh();
    };
}

#endif // DUNGEN_PREVIEW_PANEL_H

#endif