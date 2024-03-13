#ifdef TOOLS_ENABLED

#include "dungen_previewer.h"

using namespace godot;

DungenPreviewer::DungenPreviewer()
{
    set_clip_contents(true);
}
DungenPreviewer::~DungenPreviewer() {}

void DungenPreviewer::set_dungen_instance(Dungen *dungen)
{
    dungen_instance = dungen;

    if (!dungen_instance->is_connected("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete)))
    {
        dungen_instance->connect("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete));
    }
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

void DungenPreviewer::set_show_path_rectangles(bool p_show) {
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

void DungenPreviewer::_draw()
{
    if (is_queued_for_deletion()) { return; }

    set_pivot_offset(get_size() / 2);

    Vector<DungenRoom *> rooms = dungen_instance->get_map();
    Vector<DungenRoom *> trimmed_rooms = dungen_instance->get_trimmed_rooms();
    Vector<DungenRoom *> all_rooms = dungen_instance->get_all_rooms();

    if (all_rooms.size() == 0)
    {
        return;
    }

    Rect2 bounds = Rect2();

    for (int i = 0; i < all_rooms.size(); i++)
    {
        bounds.expand_to((all_rooms[i])->get_center());
    }

    bounds.grow_by(Math::max(bounds.size.x, bounds.size.y) / 2);

    Vector2 center = get_size() / 2;

    bounds.set_position(bounds.position + center);

    if (show_path_rectangles) {
        Vector<Rect2i> path_rectangles = dungen_instance->get_path_builder().get_path_rectangles();
        for (int i = 0; i < path_rectangles.size(); i++) {
            Rect2 current_rect = Rect2(path_rectangles[i]);
            current_rect.set_position(current_rect.get_position() + center);
            draw_rect(current_rect, Color::named("DODGERBLUE"));
        }
    }

    for (int i = 0; i < rooms.size(); i++)
    {
        DungenRoom *current_room = (rooms[i]);
        Rect2 rect_copy_hopefully = Rect2i(current_room->get_rectangle());
        rect_copy_hopefully.set_position(rect_copy_hopefully.get_position() + center);
        draw_rect(rect_copy_hopefully, current_room->get_color());
        draw_rect(rect_copy_hopefully, Color::named("STEELBLUE"), false, 1.0);
        draw_circle(rect_copy_hopefully.get_center(), 1.0, Color::named("BLUE"));
    }

    if (show_trimmed_rooms)
    {
        for (int i = 0; i < trimmed_rooms.size(); i++)
        {
            DungenRoom *current_room = (trimmed_rooms[i]);
            Rect2 rect_copy_hopefully = Rect2(current_room->get_rectangle());
            rect_copy_hopefully.set_position(rect_copy_hopefully.get_position() + center);
            draw_rect(rect_copy_hopefully, current_room->get_color());
            draw_rect(rect_copy_hopefully, Color::named("STEELBLUE"), false, 1.0);
        }
    }

    if (show_triangulation)
    {
        Vector<DungenTriangle> triangulation = dungen_instance->get_path_builder().get_triangulation();
        for (int i = 0; i < triangulation.size(); i++)
        {
            DungenTriangle t = triangulation[i];
            Vector2 a = t.a->get_center() + center;
            Vector2 b = t.b->get_center() + center;
            Vector2 c = t.c->get_center() + center;

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
            Vector2 a = e.a->get_center() + center;
            Vector2 b = e.b->get_center() + center;

            draw_line(a, b, Color::named("DARKORANGE"), 1.0, false);
        }
    }

    if (show_path_edges)
    {
        Vector<DungenEdge> paths = dungen_instance->get_path_builder().get_path_edges();
        for (int i = 0; i < paths.size(); i++)
        {
            DungenEdge e = paths[i];
            Vector2 a = e.a->get_center() + center;
            Vector2 b = e.b->get_center() + center;

            draw_line(a, b, Color::named("GOLD"), 1.0, false);
        }
    }

    if (show_path_rectangles) {
        Vector<Rect2i> path_rectangles = dungen_instance->get_path_builder().get_path_rectangles();
        for (int i = 0; i < path_rectangles.size(); i++) {
            Rect2 current_rect = Rect2(path_rectangles[i]);
            current_rect.set_position(current_rect.get_position() + center);
            draw_rect(current_rect, Color::named("GREEN"));
        }
    }

}

void DungenPreviewer::_bind_methods() {}

#endif // TOOLS_ENABLED