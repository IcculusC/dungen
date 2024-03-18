#ifdef TOOLS_ENABLED

#ifndef DUNGEN_EDITOR_PLUGIN_H
#define DUNGEN_EDITOR_PLUGIN_H


#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/texture2d.hpp>

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../core/dungen.h"
#include "dungen_editor.h"



namespace godot
{
    class DungenEditorPlugin : public EditorPlugin
    {
        GDCLASS(DungenEditorPlugin, EditorPlugin)

    private:
        DungenEditor *dungen_editor;

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        bool _has_main_screen() const override { return true; }
        virtual String _get_plugin_name() const override { return "Dungen"; };
        virtual void _make_visible(bool p_visible) override;
        virtual void _apply_changes() override;
        virtual void _edit(Object *p_object) override;
        virtual bool _handles(Object *p_object) const override;
        virtual Ref<Texture2D> _get_plugin_icon() const override;

        DungenEditorPlugin();
        ~DungenEditorPlugin();
    };

}
#endif // DUNGEN_EDITOR_PLUGIN_H

#endif // TOOLS_ENABLED