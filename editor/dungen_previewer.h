#ifdef TOOLS_ENABLED

#ifndef DUNGEN_PREVIEWER_H
#define DUNGEN_PREVIEWER_H

#include <godot_cpp/classes/control.hpp>

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

        bool show_trimmed_rooms;
        bool show_triangulation;
        bool show_minimum_spanning_tree;
        bool show_path_edges;
        bool show_path_rectangles;

        void _generation_complete(double p_time);

    protected:
        static void _bind_methods();

    public:
        DungenPreviewer();
        ~DungenPreviewer();

        void set_dungen_instance(Dungen *dungen);

        void set_show_trimmed_rooms(bool s_show);
        void set_show_triangulation(bool s_show);
        void set_show_minimum_spanning_tree(bool s_show);
        void set_show_path_edges(bool s_show);
        void set_show_path_rectangles(bool s_show);
        virtual void _draw() override;
    };

}

#endif

#endif // TOOLS_ENABLED