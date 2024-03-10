#include "triangulate.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Triangulation::Triangulation() : 
    points(Vector<Vector2i>()),
    corners(Vector<Vector2>()),
    super_rect(Rect2(0, 0, 1, 1)),
    triangulation(Vector<Triangle>()),
    minimum_spanning_tree(Vector<Edge>()),
    discarded_mst_edges(Vector<Edge>()) {
}

Triangulation::~Triangulation() {
}

void Triangulation::add_points(Array p_points) {
    for (int i = 0; i < p_points.size(); i++) {
        add_point((Vector2i)p_points[i]);
    }
}

void Triangulation::add_point(Vector2i p_point) {
    points.push_back(p_point);
}

void Triangulation::clear_points() {
    points.clear();
}

Vector<Vector2i> Triangulation::get_points() const {
    return points;
}

void Triangulation::create_super_rect() {
    corners.clear();
    super_rect = Rect2(-1, -1, 1, 1);
    for (int i = 0; i < points.size(); i++) {
        super_rect = super_rect.expand(points[i]);
    }
    super_rect = super_rect.grow(Math::max(super_rect.size.x, super_rect.size.y));

    Vector2 corner0 = super_rect.position;
    Vector2 corner1 = super_rect.position + Vector2(1, 0) * super_rect.size;
    Vector2 corner2 = super_rect.position + super_rect.size;
    Vector2 corner3 = super_rect.position + Vector2(0, 1) * super_rect.size;
    corners.push_back(corner0);
    corners.push_back(corner1);
    corners.push_back(corner2);
    corners.push_back(corner3);
    UtilityFunctions::print(corners[0]);
    UtilityFunctions::print(corners[1]);
    UtilityFunctions::print(corners[2]);
    UtilityFunctions::print(corners[3]);
}

void Triangulation::create_super_triangles() {
    triangulation.push_back(Triangle(corners[0], corners[1], corners[2]));
    triangulation.push_back(Triangle(corners[2], corners[3], corners[0]));
    UtilityFunctions::print(triangulation[0]);
    UtilityFunctions::print(triangulation[1]);
}

void Triangulation::find_bad_triangles(Vector2i point, Vector<Triangle> &all_triangles, Vector<Triangle> &out_triangles) {
    for (int i = 0; i < all_triangles.size(); i++) {
        Triangle triangle = all_triangles[i];
        if (triangle.is_point_in_circumfrence(point)) {
            out_triangles.push_back(triangle);
        }
    }
}

void Triangulation::make_outer_polygon(Vector<Triangle> &bad_triangles, Vector<Edge> &out_polygon, Vector<Edge> &bad_edges) {
    for (int i = 0; i < bad_triangles.size(); i++) {
        out_polygon.push_back(bad_triangles[i].ab);
        out_polygon.push_back(bad_triangles[i].bc);
        out_polygon.push_back(bad_triangles[i].ca);
    }

    // UtilityFunctions::print("TOTAL EDGES ", out_polygon.size());

    // Vector<EdgeS> duplicates;
    for (int i = 0; i < out_polygon.size(); i++) {
        for (int j = 0; j < out_polygon.size(); j++) {
            if (i == j) {
                continue;
            }
            // UtilityFunctions::print(out_polygon[i], out_polygon[j]);
            if (out_polygon[i] == out_polygon[j]) {
                bad_edges.push_back(out_polygon[i]);
                bad_edges.push_back(out_polygon[j]);
            }
        }
    }

    // UtilityFunctions::print(bad_edges.size());
    /*
    for (int i = 0; i < duplicates.size(); i++) {
        out_polygon.erase(duplicates[i]);
    }
    */
}

void Triangulation::triangulate(const int p_steps) {
    clock_t triangulate_start = clock();
    triangulation.clear();

    create_super_rect();
    create_super_triangles();
    
    for (int i = 0; i < points.size(); i++) { // points.size(); i++) {
        if (p_steps != -1 && i >= p_steps) {
            break;
        }
        const Vector2i point = points[i];
        // UtilityFunctions::print("POINT ", i, " ", point);
        // UtilityFunctions::print("STARTING TRIANGLES ", triangulation_s.size());
        Vector<Triangle> bad_triangles;

        find_bad_triangles(point, triangulation, bad_triangles);

        for (int j = 0; j < bad_triangles.size(); j++) {
            triangulation.erase(bad_triangles[j]);
        }

        // UtilityFunctions::print("BAD TRIANGLES ", bad_triangles.size());
        // UtilityFunctions::print("REMAINING TRIANGLES ", triangulation_s.size());

        Vector<Edge> bad_edges;
        Vector<Edge> polygon;
        make_outer_polygon(bad_triangles, polygon, bad_edges);
        // UtilityFunctions::print("EDGES BEFORE DELETION ", polygon.size());
        // UtilityFunctions::print("EDGES TO DELETE ", bad_edges.size());

        for (int j = 0; j < bad_edges.size(); j++) {
            polygon.erase(bad_edges[j]);
        }

        // UtilityFunctions::print("EDGES AFTER DELETION ", polygon.size());

        // UtilityFunctions::print("CONVEX POLYGON EDGE COUNT ", polygon.size());
        for (int j = 0; j < polygon.size(); j++) {
            const Edge edge = polygon[j];
            Triangle t = Triangle(point, edge.a, edge.b);

            triangulation.push_back(t);
        }

        // UtilityFunctions::print("FINISH ", i, " ", triangulation_s.size());
    }

    UtilityFunctions::print("TRIANGULATION TIME ELAPSED ", (float)(clock() - triangulate_start) / CLOCKS_PER_SEC);
}

Vector<Triangle> Triangulation::get_triangulation() const {
    return triangulation;
}

Vector2i Triangulation::find_edge_parent(HashMap<Vector2i, KruskalSubset> &subsets, Vector2i &point) const {
    if (subsets.get(point).parent == point) {
        return subsets.get(point).parent;
    }

    subsets.get(point).parent = find_edge_parent(subsets, subsets.get(point).parent);

    return subsets.get(point).parent;
}

void Triangulation::union_subsets(HashMap<Vector2i, KruskalSubset> &subsets, Vector2i &point_a, Vector2i &point_b) const {
    Vector2i a = find_edge_parent(subsets, point_a);
    Vector2i b = find_edge_parent(subsets, point_b);

    if (subsets.get(b).rank < subsets.get(a).rank) {
        subsets.get(b).parent = a;
    } else if (subsets.get(a).rank < subsets.get(b).rank) {
        subsets.get(a).parent = b;
    } else {
        subsets.get(b).parent = a;
        subsets.get(a).rank++;
    }
}

void Triangulation::build_spanning_tree() {
    clock_t start_mst = clock();
    minimum_spanning_tree.clear();
    discarded_mst_edges.clear();
    HashMap<Vector2i, KruskalSubset> subsets = HashMap<Vector2i, KruskalSubset>();
    Vector<Edge> all_edges = Vector<Edge>();

    for (int i = 0; i < triangulation.size(); i++) {
        Triangle t = triangulation[i];
        all_edges.push_back(t.ab);
        subsets.insert(t.ab.a, KruskalSubset(t.ab.a));
        all_edges.push_back(t.bc);
        subsets.insert(t.bc.a, KruskalSubset(t.bc.a));
        all_edges.push_back(t.ca);
        subsets.insert(t.ca.a, KruskalSubset(t.ca.a));
    }

    all_edges.sort();

    int V = (int)points.size();
    int j = 0;

    while (minimum_spanning_tree.size() < V - 1) {
        Edge next_edge = all_edges[j];

        if (minimum_spanning_tree.has(next_edge)) {
            j++;
            continue;
        }

        Vector2i a = find_edge_parent(subsets, next_edge.a);
        Vector2i b = find_edge_parent(subsets, next_edge.b);

        if (a != b) {
            minimum_spanning_tree.push_back(next_edge);
            union_subsets(subsets, a, b);
        } else if (!discarded_mst_edges.has(next_edge)) {
            discarded_mst_edges.push_back(next_edge);
        }

        j++;
    }

    for (j = j; j < all_edges.size(); j++) {
        if (!discarded_mst_edges.has(all_edges[j])) {
            discarded_mst_edges.push_back(all_edges[j]);
        }
    }

    UtilityFunctions::print("MINIMUM SPANNING TREE TIME ELAPSED ", (float)(clock() - start_mst) / CLOCKS_PER_SEC);
}

Vector<Edge> Triangulation::get_minimum_spanning_tree() const {
    return minimum_spanning_tree;
}

Vector<Edge> Triangulation::get_discarded_mst_edges() const {
    return discarded_mst_edges;
}