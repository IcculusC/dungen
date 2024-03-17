#ifndef DUNGEN_ROOM_H
#define DUNGEN_ROOM_H

#include <godot_cpp/classes/object.hpp>

#include <godot_cpp/templates/safe_refcount.hpp>

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot
{
    class DungenRoom
    {
    private:
        struct RoomData {
            SafeRefCount refcount;

            Color color;
            Rect2i rectangle;
        };

        RoomData *room_data = nullptr;
        void unref();

    protected:
        static void _bind_methods();

    public:
        DungenRoom();
        DungenRoom(const Vector2i &p_vector);
        DungenRoom(const Rect2i &p_rectangle);
        DungenRoom(const DungenRoom &p_room);
        ~DungenRoom();

        const double get_area() const;
        const Vector2 get_center() const;
        const Vector2 get_size() const;

        void set_position(const Vector2i p_position);
        const Vector2 get_position() const;

        void set_color(const Color p_color);
        const Color get_color() const;

        void set_rectangle(const Rect2i p_rectangle);
        const Rect2i get_rectangle() const;

        bool intersects(const DungenRoom &p_other) const;

        bool operator==(const DungenRoom &p_other) const;
        bool operator!=(const DungenRoom &p_other) const;

        void operator=(const DungenRoom &p_room);

        operator Variant() const
        {
            Dictionary result;
            result["color"] = room_data->color;
            result["rectangle"] = room_data->rectangle;
            return result;
        }

        operator Vector2i() const
        {
            return room_data->rectangle.get_center();
        }
    };

}

#endif // DUNGEN_ROOM_H