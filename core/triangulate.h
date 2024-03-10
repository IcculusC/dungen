#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

struct _NO_DISCARD_ Edge {
    Vector2i a;
    Vector2i b;

    const Vector2 get_center() const { return (a + b) * (double)0.5; }
    const double get_length() const { return Vector2(a).distance_squared_to(b); }

    Edge() {}
    Edge(const Vector2i &a, const Vector2i &b) :
        a(a),
        b(b) {
    }
    
    bool operator==(const Edge &other) const {
        return (a == other.a && b == other.b) ||
            (a == other.b && b == other.a);
    }

    bool operator!=(const Edge &other) const {
        return (a != other.a || b != other.b) ||
           (a != other.b || b != other.a);
    }

    bool operator<(const Edge &other) const {
        return get_length() < other.get_length();
    }

    operator Variant() const {
        Dictionary result = Dictionary();
        /*
        result.push_back(a);
        result.push_back(b);
        */
        result["a"] = a;
        result["b"] = b;
        return result;
    };
};

struct _NO_DISCARD_ Triangle {
    Vector2i a;
    Vector2i b;
    Vector2i c;

    Edge ab;
    Edge bc;
    Edge ca;

    Vector2 center;
    double radius_squared;

    const Vector2 &get_center() const { return center; };
    
    inline void recalculate_circumfrence() {
        double _ab = a.length_squared();
        double _cd = b.length_squared();
        double _ef = c.length_squared();

        Vector2i _cmb = c - b;
        Vector2i _amc = a - c;
        Vector2i _bma = b - a;

        Vector2 circumfrence = Vector2(
            (_ab * _cmb.y + _cd * _amc.y + _ef * _bma.y) / (a.x * _cmb.y + b.x * _amc.y + c.x * _bma.y),
            (_ab * _cmb.x + _cd * _amc.x + _ef * _bma.x) / (a.y * _cmb.x + b.y * _amc.x + c.y * _bma.x)
        );
        center = circumfrence * 0.5;
        radius_squared = Vector2(a).distance_squared_to(center);
    }

    inline bool is_point_in_circumfrence(const Vector2i &p_point) const {
        return center.distance_squared_to(Vector2(p_point)) < radius_squared;
    }

    Triangle() {}
    Triangle(const Vector2i &a, const Vector2i &b, const Vector2i &c) :
        a(a),
        b(b),
        c(c),
        ab(Edge(a, b)),
        bc(Edge(b, c)),
        ca(Edge(c, a)) {
            recalculate_circumfrence();
    };


    operator Variant() const {
        Dictionary results = Dictionary();
        /*
        results.append(ab);
        results.append(bc);
        results.append(ca);
        */
        results["ab"] = ab;
        results["bc"] = bc;
        results["ca"] = ca;
        return results;
    };

    bool operator==(const Triangle &other) const {
        return (a == other.a && b == other.b && c == other.c) ||
            (a == other.b && b == other.c && c == other.a) ||
            (a == other.c && b == other.a && c == other.b) ||
            (a == other.a && b == other.c && c == other.b) ||
            (a == other.b && b == other.a && c == other.c) ||
            (a == other.c && b == other.b && c == other.a);
    }

    bool operator!=(const Triangle &other) const {
        return !center.is_equal_approx(other.center) && !
        ((a == other.a && b == other.b && c == other.c) ||
            (a == other.b && b == other.c && c == other.a) ||
            (a == other.c && b == other.a && c == other.b) ||
            (a == other.a && b == other.c && c == other.b) ||
            (a == other.b && b == other.a && c == other.c) ||
            (a == other.c && b == other.b && c == other.a));
    }
};

struct _NO_DISCARD_ KruskalSubset {
    Vector2i parent;
    int rank;

    KruskalSubset(Vector2i &parent) :
        parent(parent),
        rank(0) {
    }
};

class Triangulation	{

private:
Vector<Vector2i> points;
Vector<Vector2> corners;
Rect2 super_rect;
Vector<Triangle> triangulation;
Vector<Edge> minimum_spanning_tree;
Vector<Edge> discarded_mst_edges;

void create_super_rect();
void create_super_triangles();
void find_bad_triangles(Vector2i point, Vector<Triangle> &all_triangles, Vector<Triangle> &out_triangles);
void make_outer_polygon(Vector<Triangle> &bad_triangles, Vector<Edge> &out_polygon, Vector<Edge> &bad_edges);

Vector2i find_edge_parent(HashMap<Vector2i, KruskalSubset> &subsets, Vector2i &point) const;
void union_subsets(HashMap<Vector2i, KruskalSubset> &subsets, Vector2i &point_a, Vector2i &point_b) const;

protected:
// static void _bind_methods();

public:
Triangulation();
~Triangulation();

void add_points(Array p_points);
void add_point(Vector2i p_point);
void clear_points();
Vector<Vector2i> get_points() const;

void triangulate(const int p_steps);
Vector<Triangle> get_triangulation() const;
void build_spanning_tree();
Vector<Edge> get_minimum_spanning_tree() const;
Vector<Edge> get_discarded_mst_edges() const;

};

}

#endif