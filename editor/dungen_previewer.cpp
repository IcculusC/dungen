#ifdef TOOLS_ENABLED

#include "dungen_previewer.h"
#include "dungen_editor_macros.h"

using namespace godot;

DungenPreviewer::DungenPreviewer()
    : grid_size(8.0), show_grid(true), show_path_rectangles(true)
{
    for (auto &a : EDITOR_THEME()->get_font_list("EditorFonts"))
    {
        UtilityFunctions::print(a);
    }

    set_clip_contents(true);
}
DungenPreviewer::~DungenPreviewer()
{
    dungen_instance = nullptr;
}

void DungenPreviewer::set_dungen_instance(Dungen *dungen)
{
    ERR_FAIL_COND_MSG(!dungen, "Provided Dungen is null.");

    dungen_instance = dungen;

    if (!dungen_instance->is_connected("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete)))
    {
        dungen_instance->connect("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete));
    }
    queue_redraw();
}

void DungenPreviewer::set_zoom(const Vector2 &p_zoom)
{
    if (zoom == p_zoom)
    {
        return;
    }
    zoom = p_zoom.clamp(Vector2(0.25, 0.25), Vector2(10, 10));
    emit_signal("zoom_changed");
    queue_redraw();
}

void DungenPreviewer::set_grid_size(const double &p_size)
{
    if (grid_size == p_size)
    {
        return;
    }
    grid_size = p_size;
    queue_redraw();
}

void DungenPreviewer::set_show_grid(bool p_show)
{
    if (show_grid == p_show)
    {
        return;
    }
    show_grid = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_all(bool p_show)
{
    if (show_all == p_show)
    {
        return;
    }
    show_all = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_trimmed_rooms(bool p_show)
{
    if (show_trimmed_rooms == p_show)
    {
        return;
    }
    show_trimmed_rooms = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_triangulation(bool p_show)
{
    if (show_triangulation == p_show)
    {
        return;
    }
    show_triangulation = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_minimum_spanning_tree(bool p_show)
{
    if (show_minimum_spanning_tree == p_show)
    {
        return;
    }
    show_minimum_spanning_tree = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_path_edges(bool p_show)
{
    if (show_path_edges == p_show)
    {
        return;
    }
    show_path_edges = p_show;
    queue_redraw();
}

void DungenPreviewer::set_show_path_rectangles(bool p_show)
{
    if (show_path_rectangles == p_show)
    {
        return;
    }
    show_path_rectangles = p_show;
    queue_redraw();
}

void DungenPreviewer::_generation_complete(double p_time)
{
    queue_redraw();
}

void DungenPreviewer::refresh()
{
    queue_redraw();
}

void DungenPreviewer::_draw()
{
    if (is_queued_for_deletion())
    {
        return;
    }

    ERR_FAIL_COND_MSG(!dungen_instance, "Dungen is null.");

    Vector2 control_size = get_size();
    Vector2 control_center = control_size / 2.0;

    set_pivot_offset(control_center);

    draw_set_transform(control_center + drag_origin, 0, zoom);

    Ref<Font> theme_italic = EDITOR_THEME()->get_font("doc_italic", "EditorFonts");

    double zoom_factor = Math::max(1.0, 1.0 / zoom.x);

    if (show_grid)
    {
        draw_line(Vector2(0, -control_size.y) * zoom_factor, Vector2(0, control_size.y) * zoom_factor, Color(0.1, 1.0, 0.1, 0.3), 1.0);
        for (int i = grid_size; i < control_size.x / 2; i += grid_size)
        {
            Vector2 start = Vector2(i, -control_size.y / 2) * zoom_factor;
            Vector2 end = Vector2(i, control_size.y / 2) * zoom_factor;
            draw_line(start, end, Color(0.7, 0.1, 1, 0.2), 1.0);
            draw_line(Vector2(-1, 1) * start, Vector2(-1, 1) * end, Color(0.7, 0.1, 1, 0.2), 1.0);
        }

        draw_line(Vector2(-control_size.x, 0) * zoom_factor, Vector2(control_size.x, 0) * zoom_factor, Color(1.0, 0.1, 0.1, 0.4), 1.0);
        for (int i = grid_size; i < control_size.x / 2; i += grid_size)
        {
            Vector2 start = Vector2(-control_size.x / 2, i) * zoom_factor;
            Vector2 end = Vector2(control_size.x / 2, i) * zoom_factor;
            draw_line(start, end, Color(0.7, 0.1, 1, 0.2), 1.0);
            draw_line(Vector2(1, -1) * start, Vector2(1, -1) * end, Color(0.7, 0.1, 1, 0.2), 1.0);
        }

        draw_circle(Vector2i(0, 0), 1.0, Color::named("GOLD"));
    }

    Vector<DungenRoom *> rooms = dungen_instance->get_map_rooms();
    Vector<DungenRoom *> trimmed_rooms = dungen_instance->get_trimmed_rooms();
    Vector<DungenRoom *> all_rooms = dungen_instance->get_all_rooms();

    if (all_rooms.size() == 0)
    {
        return;
    }

    Rect2 bounds = Rect2();

    for (int i = 0; i < all_rooms.size(); i++)
    {
        bounds = bounds.merge(all_rooms[i]->get_rectangle());
    }

    // bounds.grow_by(Math::max(bounds.size.x, bounds.size.y) / 2);
    bounds.grow_by(10.0);

    draw_string(theme_italic, Vector2(bounds.position.x + bounds.size.x / 2 - 15, bounds.position.y - 8), itos(bounds.size.x), HORIZONTAL_ALIGNMENT_CENTER, 30, 10);
    draw_string(theme_italic, Vector2(bounds.position.x + bounds.size.x + 8, bounds.position.y + bounds.size.y / 2), itos(bounds.size.y), HORIZONTAL_ALIGNMENT_CENTER, -1, 10);
    draw_rect(bounds, Color(0.5, 0.8, 0.2, .3));
    draw_rect(bounds, Color(0.5, 0.8, 0.2, .8).darkened(0.2), false, 1.0);

    if (show_path_rectangles)
    {
        Vector<Rect2i> path_rectangles = dungen_instance->get_path_builder().get_path_rectangles();
        for (int i = 0; i < path_rectangles.size(); i++)
        {
            Rect2 current_rect = path_rectangles[i];
            draw_rect(current_rect, Color::named("DODGERBLUE"));
        }
    }

    if (show_all)
    {
        for (int i = 0; i < all_rooms.size(); i++)
        {
            DungenRoom *current_room = all_rooms[i];
            Rect2i rect = current_room->get_rectangle();
            Color color = current_room->get_color();
            draw_rect(rect, color);
            draw_rect(rect, color.darkened(0.2), false, 1.0);
        }
    }

    for (int i = 0; i < rooms.size(); i++)
    {
        DungenRoom *current_room = rooms[i];
        Rect2 rect = current_room->get_rectangle();
        Color color = current_room->get_color();
        draw_rect(rect, current_room->get_color());
        draw_rect(rect, color.darkened(0.2), false, 1.0);
    }

    if (show_trimmed_rooms)
    {
        for (int i = 0; i < trimmed_rooms.size(); i++)
        {
            DungenRoom *current_room = trimmed_rooms[i];
            Rect2i rect = current_room->get_rectangle();
            Color color = current_room->get_color();
            draw_rect(rect, color);
            draw_rect(rect, color.darkened(0.2), false, 1.0);
        }
    }

    if (show_triangulation)
    {
        Vector<DungenTriangle> triangulation = dungen_instance->get_path_builder().get_triangulation();
        for (int i = 0; i < triangulation.size(); i++)
        {
            DungenTriangle t = triangulation[i];
            Vector2 a = t.a->get_center();
            Vector2 b = t.b->get_center();
            Vector2 c = t.c->get_center();

            draw_line(a, b, Color::named("LIGHTCYAN"), 1.0, false);
            draw_line(b, c, Color::named("LIGHTCYAN"), 1.0, false);
            draw_line(c, a, Color::named("LIGHTCYAN"), 1.0, false);
        }
    }

    if (show_minimum_spanning_tree)
    {
        Vector<DungenEdge> minimum_spanning_tree = dungen_instance->get_path_builder().get_minimum_spanning_tree();
        for (int i = 0; i < minimum_spanning_tree.size(); i++)
        {
            DungenEdge e = minimum_spanning_tree[i];
            Vector2 a = e.a->get_center();
            Vector2 b = e.b->get_center();

            draw_line(a, b, Color::named("DARKORANGE"), 1.0, false);
        }
    }

    if (show_path_edges)
    {
        Vector<DungenEdge> paths = dungen_instance->get_path_builder().get_path_edges();
        for (int i = 0; i < paths.size(); i++)
        {
            DungenEdge e = paths[i];
            Vector2 a = e.a->get_center();
            Vector2 b = e.b->get_center();

            draw_line(a, b, Color::named("GOLD"), 1.0, false);
        }
    }
}

void DungenPreviewer::_gui_input(const Ref<InputEvent> &event)
{
    Ref<InputEventMouseButton> mb = event;

    if (mb.is_valid())
    {
        Vector2 scroll_vec = Vector2((mb->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_RIGHT) - (mb->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_LEFT), (mb->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_DOWN) - (mb->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_UP));

        if (scroll_vec != Vector2(0, 0) && mb->is_pressed())
        {
            if (scroll_vec.y != 0)
            {
                Vector2 zoom_delta = scroll_vec.y >= 0 ? Vector2(0.25, 0.25) : Vector2(-0.25, -0.25);
                set_zoom(zoom + zoom_delta);
            }
        }

        bool is_drag_event = mb->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT;

        if (is_drag_event)
        {
            if (mb->is_pressed())
            {
                dragging = true;
            }
            else
            {
                dragging = false;
            }
        }
    }

    Ref<InputEventMagnifyGesture> magnify_gesture = event;
    if (magnify_gesture.is_valid())
    {
        UtilityFunctions::print(magnify_gesture->get_factor());
    }

    Ref<InputEventMouseMotion> mm = event;

    if (mm.is_valid())
    {
        if (dragging)
        {
            drag_origin += mm->get_relative();
        }
    }

    queue_redraw();
}

void DungenPreviewer::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("zoom_changed"));
}

#endif // TOOLS_ENABLED