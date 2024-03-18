#ifdef TOOLS_ENABLED

#ifndef DUNGEN_EDITOR_H
#define DUNGEN_EDITOR_H

#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/check_button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/h_split_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include "../core/dungen_config.h"
#include "../core/dungen.h"

#include "dungen_editor_macros.h"
#include "components/dungen_editor_toolbar.h"
#include "dungen_preview_panel.h"


namespace godot
{
    class DungenEditor : public Control
    {
        GDCLASS(DungenEditor, Control)

        friend class DungenEditorPlugin;

    private:
        EditorPlugin *plugin;
        Timer *animation_timer;
        Dungen *dungen_instance;

        FileDialog *save_dialog;
        FileDialog *load_dialog;

        VBoxContainer *main_layout;
        Button *header;

        DungenEditorToolbar *dungen_editor_toolbar;

        HSplitContainer *hsc;

        DungenPreviewPanel *dungen_preview_panel;

        PanelContainer *dungen_preview_sidebar_panel;
        VBoxContainer *dungen_preview_sidebar_layout;

        CheckButton *show_generation_animation_btn;
        CheckButton *show_path_rectangles_btn;
        CheckButton *show_trimmed_rooms_btn;
        CheckButton *show_triangulation_btn;
        CheckButton *show_minimum_spanning_tree_btn;
        CheckButton *show_path_edges_btn;

        bool show_generation_animation;

        void _initialize_dialogs();
        void _initialize_main_layout();

        void _config_changed();

        void _new_config();
        void _save_config(String p_path);
        void _on_save_pressed();
        void _load_config(String p_path);
        void _show_file_dialog(FileDialog *p_dialog) { p_dialog->popup_centered_ratio(); } // { p_dialog->popup_centered_clamped(Size2i(700, 500), 0.8f); };

        void _edit_dungen_config(Ref<DungenConfig> config);
        void _regenerate();
        void _step();

        void _set_show_generation_animation(bool p_show);

        void _redraw();

    protected:
        static void _bind_methods();

        void _notification(int p_what);

    public:
        DungenEditor();
        ~DungenEditor();

        void set_plugin(EditorPlugin *p_plugin) { plugin = p_plugin; };
    };

}

#endif // DUNGEN_EDITOR_H

#endif // TOOLS_ENABLED