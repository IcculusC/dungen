#ifdef TOOLS_ENABLED

#ifndef DUNGEN_EDITOR_TOOLBAR_H
#define DUNGEN_EDITOR_TOOLBAR_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/object.hpp>

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class DungenEditorToolbar : public HBoxContainer
    {
        GDCLASS(DungenEditorToolbar, HBoxContainer)

    private:
        Button *new_btn;
        Button *load_btn;
        Button *save_btn;

        void _emit_pressed(String p_which);

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        DungenEditorToolbar();
        ~DungenEditorToolbar();
    };
}

#endif // DUNGEN_EDITOR_TOOLBAR_H

#endif