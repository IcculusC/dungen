#ifndef DUNGEN_ROOM_H
#define DUNGEN_ROOM_H

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot
{

    class DungenRoom : public RefCounted
    {
        GDCLASS(DungenRoom, RefCounted)

    private:
        Color color;
        Rect2i rectangle;

    protected:
        static void _bind_methods();

    public:
        DungenRoom();
        DungenRoom(Vector2i _vector);
        DungenRoom(Rect2i _rectangle);
        ~DungenRoom();

        double get_area() const;
        Vector2 get_center() const;
        Vector2 get_size() const;

        void set_position(const Vector2i p_position);
        Vector2 get_position() const;

        void set_color(const Color p_color);
        Color get_color() const;

        void set_rectangle(const Rect2i p_rectangle);
        Rect2i get_rectangle() const;

        bool intersects(const Ref<DungenRoom> &other);

        bool operator==(const Ref<DungenRoom> &other) const;
        bool operator!=(const Ref<DungenRoom> &other) const;

        operator Variant() const;
        operator Variant();

        operator Vector2i() const;
        operator Vector2i();
    };

}

#endif // DUNGEN_ROOM_H