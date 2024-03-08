#ifdef TOOLS_ENABLED

#ifndef DUNGEN_EDITOR_PLUGIN_H
#define DUNGEN_EDITOR_PLUGIN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class DungenEditor : public Control {
    GDCLASS(DungenEditor, Control)

private:
    EditorPlugin *plugin;

protected:
    static void _bind_methods();

    void _notification(int p_what);

public:
	DungenEditor();
	~DungenEditor();

	void set_plugin(EditorPlugin *p_plugin) { plugin = p_plugin; };
};

class DungenEditorPlugin : public EditorPlugin {
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