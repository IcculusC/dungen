#include "dungen_room.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DungenRoom::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_area"), &DungenRoom::get_area);
    ClassDB::bind_method(D_METHOD("get_center"), &DungenRoom::get_center);
    ClassDB::bind_method(D_METHOD("get_rectangle"), &DungenRoom::get_rectangle);

    ClassDB::bind_method(D_METHOD("get_color"), &DungenRoom::get_color);
    ClassDB::bind_method(D_METHOD("set_color", "p_color"), &DungenRoom::set_color);
}

DungenRoom::DungenRoom() : color(Color::named("ORANGERED")),
                           rectangle(Rect2(Vector2(0, 0), Vector2(1, 1)))
{
}

DungenRoom::DungenRoom(Vector2 _vector) : color(Color::named("ORANGERED")),
                                          rectangle(Rect2(_vector - Vector2(1, 1), Vector2(2, 2)))
{
}

DungenRoom::DungenRoom(Rect2 _rectangle) : color(Color::named("ORANGERED")),
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

void DungenRoom::set_position(const Vector2 p_position)
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

void DungenRoom::set_rectangle(const Rect2 p_rectangle)
{
    rectangle = p_rectangle;
}
Rect2 DungenRoom::get_rectangle() const
{
    return rectangle;
}

bool DungenRoom::intersects(const Ref<DungenRoom> &other)
{
    return rectangle.intersects(other->get_rectangle());
}

bool DungenRoom::operator==(const Ref<DungenRoom> &other) const
{
    return rectangle == other->get_rectangle();
}

bool DungenRoom::operator!=(const Ref<DungenRoom> &other) const
{
    return rectangle != other->get_rectangle();
}

DungenRoom::operator Variant() const
{
    Dictionary result;
    result["color"] = color;
    result["rectangle"] = rectangle;
    return result;
}

DungenRoom::operator Variant()
{
    Dictionary result;
    result["color"] = color;
    result["rectangle"] = rectangle;
    return result;
}
