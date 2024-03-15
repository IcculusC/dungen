#include "dungen_room.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DungenRoom::_bind_methods()
{
}

DungenRoom::DungenRoom()
    : color(Color::named("ORANGERED")),
      rectangle(Rect2(Vector2i(0, 0), Vector2i(1, 1)))
{
}

DungenRoom::DungenRoom(Vector2i _vector)
    : color(Color::named("ORANGERED")),
      rectangle(Rect2i(_vector - Vector2i(1, 1), Vector2(2, 2)))
{
}

DungenRoom::DungenRoom(Rect2i _rectangle)
    : color(Color::named("ORANGERED")),
      rectangle(_rectangle)
{
}

DungenRoom::~DungenRoom()
{
}

double DungenRoom::get_area() const
{
    return rectangle.get_area();
}

Vector2 DungenRoom::get_center() const
{
    return rectangle.get_center();
}

Vector2 DungenRoom::get_size() const
{
    return rectangle.get_size();
}

void DungenRoom::set_position(const Vector2i p_position)
{
    rectangle.set_position(p_position);
}

Vector2 DungenRoom::get_position() const
{
    return rectangle.position;
}

void DungenRoom::set_color(const Color p_color)
{
    color = p_color;
}

Color DungenRoom::get_color() const
{
    return color;
}

void DungenRoom::set_rectangle(const Rect2i p_rectangle)
{
    rectangle = p_rectangle;
}

Rect2i DungenRoom::get_rectangle() const
{
    return rectangle;
}

bool DungenRoom::intersects(DungenRoom *other)
{
    return rectangle.intersects(other->get_rectangle());
}

bool DungenRoom::operator==(DungenRoom *other) const
{
    return rectangle == other->get_rectangle();
}

bool DungenRoom::operator!=(DungenRoom *other) const
{
    return rectangle != other->get_rectangle();
}
