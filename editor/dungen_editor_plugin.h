#ifdef TOOLS_ENABLED

#ifndef DUNGEN_EDITOR_PLUGIN_H
#define DUNGEN_EDITOR_PLUGIN_H

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/color_rect.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/h_split_container.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../core/dungen.h"
#include "dungen_previewer.h"

namespace godot
{

    class DungenEditor : public Control
    {
        GDCLASS(DungenEditor, Control)

        friend class DungenEditorPlugin;

    private:
        EditorPlugin *plugin;
        Dungen *dungen_instance;

        VBoxContainer *vbox;
        Button *header;

        FileDialog *save_dialog;
        FileDialog *load_dialog;

        Button *new_btn;
        Button *load_btn;
        Button *save_btn;

        HSplitContainer *hsc;

        PanelContainer *dungen_texture_panel;

        VBoxContainer *side_bar_vbox;

        Button *zoom_less_btn;
        Button *zoom_reset_btn;
        Button *zoom_more_btn;
    
        DungenPreviewer *dungen_previewer;

        void _config_changed();

        void _new_config();
        void _save_config(String p_path);
        void _on_save_pressed();
        void _load_config(String p_path);
        void _show_file_dialog(FileDialog *p_dialog) { p_dialog->popup_centered_ratio(); } // { p_dialog->popup_centered_clamped(Size2i(700, 500), 0.8f); };

        void _edit_dungen_config(Ref<DungenConfig> config);
        void _regenerate();

        void _generation_complete(double p_time);

        void _reset_zoom();
        void _update_zoom_reset_button();
        void _zoom_editor(Vector2 amount);

        void _redraw();

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        DungenEditor();
        ~DungenEditor();

        void set_plugin(EditorPlugin *p_plugin) { plugin = p_plugin; };
    };

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