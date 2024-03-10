#include "dungen_path_builder.h"

using namespace godot;

DungenPathBuilder::DungenPathBuilder() : super_rect(Rect2(0, 0, 1, 1)) {}
DungenPathBuilder::~DungenPathBuilder() {}

void DungenPathBuilder::add_rooms(const Vector<DungenRoom *> &p_rooms)
{
    rooms.append_array(p_rooms);
}

void DungenPathBuilder::add_room(DungenRoom *p_room)
{
    rooms.push_back(p_room);
}

void DungenPathBuilder::clear_rooms()
{
    rooms.clear();
}

void DungenPathBuilder::triangulate()
{
    triangulation.clear();
    corners.clear();

    // CREATE INITIAL GEOMETRY
    super_rect = Rect2(-1, -1, 1, 1);
    for (int i = 0; i < rooms.size(); i++)
    {
        super_rect.expand_to(rooms[i]->get_center());
    }

    super_rect.grow_by(Math::max(super_rect.size.x, super_rect.size.y) / 2);

    Vector2i corner0 = super_rect.position;
    Vector2i corner1 = super_rect.position + (Vector2i(1, 0) * super_rect.size);
    Vector2i corner2 = super_rect.position + super_rect.size;
    Vector2i corner3 = super_rect.position + (Vector2i(0, 1) * super_rect.size);

    corners.push_back(memnew(DungenRoom(corner0)));
    corners.push_back(memnew(DungenRoom(corner1)));
    corners.push_back(memnew(DungenRoom(corner2)));
    corners.push_back(memnew(DungenRoom(corner3)));

    clock_t triangulate_start = clock();

    triangulation.push_back(DungenTriangle(corners[0], corners[1], corners[2]));
    triangulation.push_back(DungenTriangle(corners[2], corners[3], corners[0]));
    // CREATE INITIAL GEOMETRY

    // MAIN LOOP
    for (int room_number = 0; room_number < rooms.size(); room_number++)
    {
        DungenRoom *room = rooms[room_number];
        Vector<DungenTriangle> bad_triangles;

        // find triangles whos circumcircle contains the rooms center
        for (int i = 0; i < triangulation.size(); i++)
        {
            DungenTriangle current_triangle = triangulation[i];

            if (current_triangle.is_point_in_circumfrence(room->get_center()))
            {
                bad_triangles.push_back(current_triangle);
            }
        }

        // delete the bad ones from the overall triangulation
        for (int i = 0; i < bad_triangles.size(); i++)
        {
            triangulation.erase(bad_triangles[i]);
        }

        Vector<DungenEdge> bad_edges;
        Vector<DungenEdge> outer_polygon;

        for (int i = 0; i < bad_triangles.size(); i++)
        {
            outer_polygon.push_back(bad_triangles[i].ab);
            outer_polygon.push_back(bad_triangles[i].bc);
            outer_polygon.push_back(bad_triangles[i].ca);
        }

        for (int i = 0; i < outer_polygon.size(); i++)
        {
            for (int j = 0; j < outer_polygon.size(); j++)
            {
                if (i == j)
                {
                    continue;
                }
                if (outer_polygon[i] == outer_polygon[j])
                {
                    bad_edges.push_back(outer_polygon[i]);
                    bad_edges.push_back(outer_polygon[j]);
                }
            }
        }

        for (int i = 0; i < bad_edges.size(); i++)
        {
            outer_polygon.erase(bad_edges[i]);
        }

        // attempting to not add duplicate edges at all, but we'll see
        /*
        for (int t = 0; t < bad_triangles.size(); t++) {
            if (!outer_polygon.has(bad_triangles[t].ab)) {
               outer_polygon.push_back(bad_triangles[t].ab);
            }
            if (!outer_polygon.has(bad_triangles[t].bc)) {
               outer_polygon.push_back(bad_triangles[t].bc);
            }
            if (!outer_polygon.has(bad_triangles[t].ca)) {
               outer_polygon.push_back(bad_triangles[t].ca);
            }
        }
        */

        for (int i = 0; i < outer_polygon.size(); i++)
        {
            DungenEdge current_edge = outer_polygon[i];
            DungenTriangle t = DungenTriangle(room, current_edge.a, current_edge.b);
            triangulation.push_back(t);
        }
    }

    Vector<DungenTriangle> corner_triangles;

    for (int t = 0; t < triangulation.size(); t++)
    {
        DungenTriangle current_triangle = triangulation[t];

        if (corners.has(current_triangle.a) || corners.has(current_triangle.b) || corners.has(current_triangle.c))
        {
            corner_triangles.push_back(current_triangle);
        }
    }

    for (int t = 0; t < corner_triangles.size(); t++)
    {
        triangulation.erase(corner_triangles[t]);
    }
    // MAIN LOOP

    UtilityFunctions::print("TRIANGULATION TIME ELAPSED ", (float)(clock() - triangulate_start) / CLOCKS_PER_SEC);
}

DungenRoom *DungenPathBuilder::_find_edge_parent(HashMap<DungenRoom *, DungenDisjoinSet> &subsets, DungenRoom *room)
{
    if (subsets.get(room).parent == room)
    {
        return subsets.get(room).parent;
    }

    subsets.get(room).parent = _find_edge_parent(subsets, subsets.get(room).parent);

    return subsets.get(room).parent;
}

void DungenPathBuilder::_union_subsets(HashMap<DungenRoom *, DungenDisjoinSet> &subsets, DungenRoom *room_a, DungenRoom *room_b)
{
    DungenRoom *parent_a = _find_edge_parent(subsets, room_a);
    DungenRoom *parent_b = _find_edge_parent(subsets, room_b);

    if (subsets.get(parent_b).rank < subsets.get(parent_a).rank)
    {
        subsets.get(parent_b).parent = parent_a;
    }
    else if (subsets.get(parent_a).rank < subsets.get(parent_b).rank)
    {
        subsets.get(parent_a).parent = parent_b;
    }
    else
    {
        subsets.get(parent_b).parent = parent_a;
        subsets.get(parent_a).rank++;
    }
}

void DungenPathBuilder::find_minimum_spanning_tree()
{
    minimum_spanning_tree.clear();

    HashMap<DungenRoom *, DungenDisjoinSet> subsets;
    Vector<DungenEdge> all_edges;

    for (int i = 0; i < triangulation.size(); i++)
    {
        DungenTriangle t = triangulation[i];
        all_edges.push_back(t.ab);
        subsets.insert(t.ab.a, DungenDisjoinSet(t.ab.a));
        all_edges.push_back(t.bc);
        subsets.insert(t.bc.a, DungenDisjoinSet(t.bc.a));
        all_edges.push_back(t.ca);
        subsets.insert(t.ca.a, DungenDisjoinSet(t.ca.a));
    }

    all_edges.sort();

    int V = rooms.size();
    int j = 0;

    clock_t start_mst = clock();

    while (minimum_spanning_tree.size() < V - 1)
    {
        DungenEdge next_edge = all_edges[j];

        if (minimum_spanning_tree.has(next_edge))
        {
            j++;
            continue;
        }

        DungenRoom *room_a = _find_edge_parent(subsets, next_edge.a);
        DungenRoom *room_b = _find_edge_parent(subsets, next_edge.b);

        if (room_a != room_b)
        {
            minimum_spanning_tree.push_back(next_edge);
            _union_subsets(subsets, room_a, room_b);
        }
        /*
        else if (!discarded_mst_edges.has(next_edge)) {
            discarded_mst_edges.push_back(next_edge);
        }
        */

        j++;
    }

    UtilityFunctions::print("MINIMUM SPANNING TREE TIME ELAPSED ", (float)(clock() - start_mst) / CLOCKS_PER_SEC);
}

DungenPathBuilder::operator Variant() const
{
    Array result_triangulation = Array();
    for (int i = 0; i < triangulation.size(); i++)
    {
        result_triangulation.push_back(triangulation[i]);
    }
    Array result_mst = Array();
    for (int i = 0; i < minimum_spanning_tree.size(); i++)
    {
        result_mst.push_back(minimum_spanning_tree[i]);
    }
    Dictionary result;
    result["triangulation"] = result_triangulation;
    result["minimum_spanning_tree"] = result_mst;
    return result;
}