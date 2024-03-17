#include "dungen_room.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DungenRoom::_bind_methods()
{
}

DungenRoom::DungenRoom()
    : DungenRoom(Vector2i(0, 0))
{
}

DungenRoom::DungenRoom(const Vector2i &p_vector)
    : DungenRoom(Rect2i(p_vector - Vector2i(1, 1), Vector2i(2, 2)))
{
}

DungenRoom::DungenRoom(const Rect2i &p_rectangle)
{
    room_data = memnew(RoomData);
    room_data->refcount.init();
    set_color(Color::named("ORANGERED"));
    set_rectangle(p_rectangle);
}

DungenRoom::DungenRoom(const DungenRoom &p_room) {
    if (p_room.room_data && p_room.room_data->refcount.ref()) {
        room_data = p_room.room_data;
    }
}

DungenRoom::~DungenRoom()
{
    unref();
}

void DungenRoom::unref() {
    if (room_data && room_data->refcount.unref()) {
        memdelete(room_data);
    }
    room_data = nullptr;
}

const double DungenRoom::get_area() const
{
    return room_data->rectangle.get_area();
}

const Vector2 DungenRoom::get_center() const
{
    return room_data->rectangle.get_center();
}

const Vector2 DungenRoom::get_size() const
{
    return room_data->rectangle.get_size();
}

void DungenRoom::set_position(const Vector2i p_position)
{
    room_data->rectangle.set_position(p_position);
}

const Vector2 DungenRoom::get_position() const
{
    return room_data->rectangle.position;
}

void DungenRoom::set_color(const Color p_color)
{
    room_data->color = p_color;
}

const Color DungenRoom::get_color() const
{
    return room_data->color;
}

void DungenRoom::set_rectangle(const Rect2i p_rectangle)
{
    room_data->rectangle = p_rectangle;
}

const Rect2i DungenRoom::get_rectangle() const 
{
    return room_data->rectangle;
}

bool DungenRoom::intersects(const DungenRoom &p_other) const
{
    return room_data->rectangle.intersects(p_other.get_rectangle());
}

bool DungenRoom::operator==(const DungenRoom &p_other) const
{
    return room_data->rectangle == p_other.get_rectangle();
}

bool DungenRoom::operator!=(const DungenRoom &p_other) const
{
    return room_data->rectangle != p_other.get_rectangle();
}

void DungenRoom::operator=(const DungenRoom &p_room) {
    if (*this == p_room) {
        return;
    }

    unref();

    if (p_room.room_data && p_room.room_data->refcount.ref()) {
        room_data = p_room.room_data;
    }
}