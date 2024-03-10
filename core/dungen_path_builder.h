#ifndef DUNGEN_PATH_BUILDER_H
#define DUNGEN_PATH_BUILDER_H

#include <godot_cpp/classes/ref.hpp>

#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "dungen_room.h"

namespace godot
{
    struct DungenEdge
    {
        DungenRoom *a;
        DungenRoom *b;

        const Vector2i get_center() const { return (a->get_center() + b->get_center()) / 2.0; }
        const double get_length() const { return Vector2(a->get_center()).distance_squared_to(b->get_center()); }

        DungenEdge() {}
        DungenEdge(DungenRoom *a, DungenRoom *b) : a(a),
                                                   b(b)
        {
        }

        operator Variant() const
        {
            Dictionary results;
            results["a"] = *a;
            results["b"] = *b;
            return results;
        };

        bool operator==(const DungenEdge &other) const
        {
            return (a == other.a && b == other.b) ||
                   (a == other.b && b == other.a);
        }

        bool operator!=(const DungenEdge &other) const
        {
            return (a != other.a || b != other.b) ||
                   (a != other.b || b != other.a);
        }

        bool operator<(const DungenEdge &other) const
        {
            return get_length() < other.get_length();
        }

        bool operator>(const DungenEdge &other) const
        {
            return get_length() > other.get_length();
        }
    };

    struct DungenTriangle
    {
        DungenRoom *a;
        DungenRoom *b;
        DungenRoom *c;

        DungenEdge ab;
        DungenEdge bc;
        DungenEdge ca;

        Vector2 center;
        double radius_squared;

        const Vector2 get_center() const { return center; };

        inline DungenRoom *get_a() const { return a; }
        inline DungenRoom *get_b() const { return b; }
        inline DungenRoom *get_c() const { return c; }

        void recalculate_circumfrence()
        {
            Vector2 _a = a->get_center();
            Vector2 _b = b->get_center();
            Vector2 _c = c->get_center();

            double _ab = _a.length_squared();
            double _cd = _b.length_squared();
            double _ef = _c.length_squared();

            Vector2i _cmb = _c - _b;
            Vector2i _amc = _a - _c;
            Vector2i _bma = _b - _a;

            Vector2 circumfrence = Vector2(
                (_ab * _cmb.y + _cd * _amc.y + _ef * _bma.y) / (_a.x * _cmb.y + _b.x * _amc.y + _c.x * _bma.y),
                (_ab * _cmb.x + _cd * _amc.x + _ef * _bma.x) / (_a.y * _cmb.x + _b.y * _amc.x + _c.y * _bma.x));
            center = circumfrence * 0.5;
            radius_squared = Vector2(_a).distance_squared_to(center);
        }

        inline bool is_point_in_circumfrence(const Vector2i &p_point) const
        {
            return center.distance_squared_to(Vector2(p_point)) < radius_squared;
        }

        DungenTriangle() {}
        DungenTriangle(const Vector2i &a, const Vector2i &b, const Vector2i &c) : DungenTriangle(
                                                                                      (memnew(DungenRoom(a))),
                                                                                      (memnew(DungenRoom(b))),
                                                                                      (memnew(DungenRoom(c)))) {}
        DungenTriangle(DungenRoom *a, DungenRoom *b, DungenRoom *c) : a(a),
                                                                      b(b),
                                                                      c(c),
                                                                      ab(DungenEdge(a, b)),
                                                                      bc(DungenEdge(b, c)),
                                                                      ca(DungenEdge(c, a))
        {
            recalculate_circumfrence();
        };

        operator Variant() const
        {
            Dictionary results;
            results["a"] = *a;
            results["b"] = *b;
            results["c"] = *c;
            return results;
        };

        bool operator==(const DungenTriangle &other) const
        {
            return (a == other.a && b == other.b && c == other.c) ||
                   (a == other.b && b == other.c && c == other.a) ||
                   (a == other.c && b == other.a && c == other.b) ||
                   (a == other.a && b == other.c && c == other.b) ||
                   (a == other.b && b == other.a && c == other.c) ||
                   (a == other.c && b == other.b && c == other.a);
        }

        bool operator!=(const DungenTriangle &other) const
        {
            return !center.is_equal_approx(other.center) && !((a == other.a && b == other.b && c == other.c) ||
                                                              (a == other.b && b == other.c && c == other.a) ||
                                                              (a == other.c && b == other.a && c == other.b) ||
                                                              (a == other.a && b == other.c && c == other.b) ||
                                                              (a == other.b && b == other.a && c == other.c) ||
                                                              (a == other.c && b == other.b && c == other.a));
        }
    };

    struct DungenDisjoinSet
    {
        DungenRoom *parent;
        int rank;

        DungenDisjoinSet(DungenRoom *parent) : parent(parent),
                                               rank(0)
        {
        }
    };

    class DungenPathBuilder
    {
    private:
        Vector<DungenRoom *> rooms;
        Vector<DungenRoom *> corners;
        Rect2 super_rect;
        Vector<DungenTriangle> triangulation;
        Vector<DungenEdge> minimum_spanning_tree;
        Vector<DungenEdge> non_spanning_edges;

        DungenRoom *_find_edge_parent(HashMap<DungenRoom *, DungenDisjoinSet> &subsets, DungenRoom *room);
        void _union_subsets(HashMap<DungenRoom *, DungenDisjoinSet> &subsets, DungenRoom *room_a, DungenRoom *room_b);

    public:
        DungenPathBuilder();
        ~DungenPathBuilder();

        void add_rooms(const Vector<DungenRoom *> &p_rooms);
        void add_room(DungenRoom *p_room);
        void clear_rooms();

        void triangulate();
        void find_minimum_spanning_tree();

        Vector<DungenTriangle> get_triangulation() { return triangulation; };
        Vector<DungenEdge> get_minimum_spanning_tree() { return minimum_spanning_tree; };
        Vector<DungenEdge> get_path_edges();

        operator Variant() const;
    };
}

#endif // DUNGEN_PATH_BUILDER_H
