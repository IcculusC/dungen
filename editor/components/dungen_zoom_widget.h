#ifdef TOOLS_ENABLED

#ifndef DUNGEN_ZOOM_WIDGET_H
#define DUNGEN_ZOOM_WIDGET_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class DungenZoomWidget : public HBoxContainer 
    {
        GDCLASS(DungenZoomWidget, HBoxContainer)

    private:
        Button *zoom_out_btn;
        Button *zoom_reset_btn;
        Button *zoom_in_btn;
    
    protected:
        static void _bind_methods();

        void _notification(int p_what);

        void _emit_pressed(String p_which);

    public:
        DungenZoomWidget();
        ~DungenZoomWidget(); 
    };
}

#endif // DUNGEN_ZOOM_WIDGET_H

#endif