#ifdef TOOLS_ENABLED

#ifndef DUNGEN_PREVIEWER_H
#define DUNGEN_PREVIEWER_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_magnify_gesture.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/theme.hpp>

#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/vector2.hpp>

#include "../core/dungen.h"

namespace godot
{

    class DungenPreviewer : public Control
    {
        GDCLASS(DungenPreviewer, Control)

    private:
        Dungen *dungen_instance;

        Vector2 zoom;

        bool show_all;
        bool show_trimmed_rooms;
        bool show_triangulation;
        bool show_minimum_spanning_tree;
        bool show_path_edges;
        bool show_path_rectangles;

        Vector2 drag_origin = Vector2(0, 0);
        bool dragging = false;

        void _generation_complete(double p_time);

    protected:
        static void _bind_methods();

    public:
        DungenPreviewer();
        ~DungenPreviewer();

        void set_dungen_instance(Dungen *dungen);

        void set_zoom(const Vector2 &p_zoom);
        const Vector2 get_zoom() const { return zoom; };

        void set_show_all(bool p_show);
        void set_show_trimmed_rooms(bool p_show);
        void set_show_triangulation(bool p_show);
        void set_show_minimum_spanning_tree(bool p_show);
        void set_show_path_edges(bool p_show);
        void set_show_path_rectangles(bool p_show);
        virtual void _draw() override;

        void refresh();

        void _gui_input(const Ref<InputEvent> &event) override;
    };

}

#endif

#endif // TOOLS_ENABLED