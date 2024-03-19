#ifdef TOOLS_ENABLED

#define EDITOR_INTERFACE() (EditorInterface::get_singleton())

#define MAIN_SCREEN_CONTROL() (EDITOR_INTERFACE()->get_editor_main_screen())
#define EDITOR_THEME() (EDITOR_INTERFACE()->get_editor_theme())

#define EXPAND_FILL_V(m_thing) m_thing->set_v_size_flags(SIZE_EXPAND_FILL);
#define EXPAND_FILL_H(m_thing) m_thing->set_h_size_flags(SIZE_EXPAND_FILL);

#define EXPAND_FILL(m_thing) \
    EXPAND_FILL_V(m_thing)   \
    EXPAND_FILL_H(m_thing)

#define MARGIN_OVERRIDE(m_thing, m_side, m_size) m_thing->add_theme_constant_override("margin_" m_side, m_size);

#define MARGIN_OVERRIDE_ALL(m_thing, m_size)   \
    MARGIN_OVERRIDE(m_thing, "left", m_size);  \
    MARGIN_OVERRIDE(m_thing, "top", m_size);   \
    MARGIN_OVERRIDE(m_thing, "right", m_size); \
    MARGIN_OVERRIDE(m_thing, "bottom", m_size);

#define ADD_STYLEBOX_OVERRIDE(m_control, m_name, m_stylebox) (m_control->add_theme_stylebox_override(m_name, m_stylebox));
#define ADD_COLOR_OVERRIDE(m_control, m_name, m_color) (m_control->add_theme_color_override(m_name, m_color));
#define ADD_CONSTANT_OVERRIDE(m_control, m_name, m_constant) (m_control->add_theme_constant_override(m_name, m_constant));

#define RESOURCE_LOADER() (ResourceLoader::get_singleton())
#define RESOURCE_SAVER() (ResourceSaver::get_singleton())

#define RESOURCE_EXISTS(m_path, m_hint) RESOURCE_LOADER()->exists(m_path, m_hint)
#define RESOURCE_LOAD(m_path, m_hint) RESOURCE_LOADER()->load(m_path, m_hint)

#define RESOURCE_SAVE(m_res, m_path, m_flags) RESOURCE_SAVER()->save(m_res, m_path, m_flags)

#endif
