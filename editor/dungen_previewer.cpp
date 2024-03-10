#ifdef TOOLS_ENABLED

#include "dungen_previewer.h"

using namespace godot;

DungenPreviewer::DungenPreviewer() {
    set_clip_contents(true);
}
DungenPreviewer::~DungenPreviewer() {}

void DungenPreviewer::set_dungen_instance(Dungen *dungen) {
    dungen_instance = dungen;

    if (!dungen_instance->is_connected("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete)))
    {
        dungen_instance->connect("generation_complete", callable_mp(this, &DungenPreviewer::_generation_complete));
    }
}

void DungenPreviewer::set_show_trimmed_rooms(bool p_show_trimmed_rooms) {
    if (show_trimmed_rooms == p_show_trimmed_rooms) {
        return;
    }
    show_trimmed_rooms = p_show_trimmed_rooms;
    queue_redraw();
}

void DungenPreviewer::_generation_complete(double p_time)
{
    queue_redraw();
}

void DungenPreviewer::_draw() {
    set_pivot_offset(get_size() / 2);

    Array rooms = dungen_instance->get_map();
    Array trimmed_rooms = dungen_instance->get_trimmed_rooms();

    Array all_rooms = dungen_instance->get_all_rooms();

    if (all_rooms.size() == 0)
    {
        return;
    }

    Rect2 bounds = Rect2();

    for (int i = 0; i < all_rooms.size(); i++)
    {
        bounds.expand_to(Ref<DungenRoom>(all_rooms[i])->get_center());
    }

    bounds.grow_by(Math::max(bounds.size.x, bounds.size.y) / 2);

    Vector2 center = get_size() / 2;

    bounds.set_position(bounds.position + center);

    draw_rect(bounds, Color::named("CORNFLOWER_BLUE"), false, 2.0);

    for (int i = 0; i < rooms.size(); i++)
    {
        Ref<DungenRoom> current_room = Ref<DungenRoom>(rooms[i]);
        Rect2 rect_copy_hopefully = Rect2(current_room->get_rectangle());
        rect_copy_hopefully.set_position(rect_copy_hopefully.get_position() + center);
        draw_rect(rect_copy_hopefully, current_room->get_color());
        draw_rect(rect_copy_hopefully, Color::named("STEELBLUE"), false, 1.0);
        draw_circle(rect_copy_hopefully.get_center(), 1.0, Color::named("BLUE"));
    }

    if (show_trimmed_rooms) {
         for (int i = 0; i < trimmed_rooms.size(); i++)
        {
            Ref<DungenRoom> current_room = Ref<DungenRoom>(trimmed_rooms[i]);
            Rect2 rect_copy_hopefully = Rect2(current_room->get_rectangle());
            rect_copy_hopefully.set_position(rect_copy_hopefully.get_position() + center);
            draw_rect(rect_copy_hopefully, current_room->get_color());
            draw_rect(rect_copy_hopefully, Color::named("STEELBLUE"), false, 1.0);
        }   
    }

    Vector<DungenTriangle> triangulation = dungen_instance->get_path_builder().get_triangulation();
    for (int i = 0; i < triangulation.size(); i++)
    {
        DungenTriangle t = triangulation[i];
        Vector2 a = t.get_a()->get_center() + center;
        Vector2 b = t.get_b()->get_center() + center;
        Vector2 c = t.get_c()->get_center() + center;

        draw_line(a, b, Color::named("BLACK"), 1.0, false);
        draw_line(b, c, Color::named("BLACK"), 1.0, false);
        draw_line(c, a, Color::named("BLACK"), 1.0, false);
    }
}

void DungenPreviewer::_bind_methods() {}

#endif // TOOLS_ENABLED