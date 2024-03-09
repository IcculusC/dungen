#ifdef TOOLS_ENABLED

#include "dungen_previewer.h"

using namespace godot;

DungenPreviewer::DungenPreviewer() {
    set_clip_contents(true);
}
DungenPreviewer::~DungenPreviewer() {}

void DungenPreviewer::set_dungen_instance(Dungen *dungen) {
    dungen_instance = dungen;
}

void DungenPreviewer::_draw() {
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

    // Vector2 center = bounds.get_size() / 2;
    Vector2 center = get_size() / 2;

    for (int i = 0; i < all_rooms.size(); i++)
    {
        Ref<DungenRoom> current_room = Ref<DungenRoom>(all_rooms[i]);
        Rect2 rect_copy_hopefully = Rect2(current_room->get_rectangle());
        rect_copy_hopefully.set_position(rect_copy_hopefully.get_position() + center);
        draw_rect(rect_copy_hopefully, current_room->get_color());
    }
}

void DungenPreviewer::_bind_methods() {}

#endif // TOOLS_ENABLED