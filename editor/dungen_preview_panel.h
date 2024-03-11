#ifdef TOOLS_ENABLED

#ifndef DUNGEN_PREVIEW_PANEL_H
#define DUNGEN_PREVIEW_PANEL_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <godot_cpp/variant/string.hpp>

#include "../core/dungen.h"
#include "dungen_previewer.h"

namespace godot
{
    class DungenPreviewPanel : public PanelContainer
    {
        GDCLASS(DungenPreviewPanel, PanelContainer)

    private:
        VBoxContainer *vbox;

        HBoxContainer *nav;

        Button *zoom_out_btn;
        Button *zoom_reset_btn;
        Button *zoom_in_btn;

        PanelContainer *dungen_preview_wrapper;

        DungenPreviewer *dungen_previewer;

        void _reset_previewer_zoom();
        void _update_zoom_reset_button();
        void _update_previewer_zoom(Vector2 amount);

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        DungenPreviewPanel();
        ~DungenPreviewPanel();

        void set_dungen_instance(Dungen *dungen);

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