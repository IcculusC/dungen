#include "dungen_path_builder.h"

using namespace godot;

DungenPathBuilder::DungenPathBuilder() : super_rect(Rect2(0, 0, 1, 1)) {}
DungenPathBuilder::~DungenPathBuilder()
{
    reset();
}

void DungenPathBuilder::reset()
{
    rooms.clear();
    triangulation.clear();
    corners.clear();
    minimum_spanning_tree.clear();
    non_spanning_edges.clear();

    phase = START;
    current_index = 0;
}

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
    reset();
}

int DungenPathBuilder::begin()
{
    reset();
    return current_index;
}

int DungenPathBuilder::next()
{
    switch (phase)
    {
    case START:
    {
        _setup_triangulation();
        phase = TRIANGULATE;
        break;
    }
    case TRIANGULATE:
        if (triangulate_point(current_index))
        {
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
            phase = MINIMUM_SPANNING_TREE;
        }
        current_index++;
        break;
    case MINIMUM_SPANNING_TREE:
        find_minimum_spanning_tree();
        phase = COMPLETE;
        break;
    case COMPLETE:
    default:
        return -1;
    }

    return current_index;
}


void DungenPathBuilder::_setup_triangulation() {
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

    triangulation.push_back(DungenTriangle(corners[0], corners[1], corners[2]));
    triangulation.push_back(DungenTriangle(corners[2], corners[3], corners[0]));
    // CREATE INITIAL GEOMETRY
}

bool DungenPathBuilder::triangulate_point(int i)
{
    if (i == rooms.size())
    {
        return true;
    }

    DungenRoom *room = rooms[i];
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
   
    for (int i = 0; i < outer_polygon.size(); i++)
    {
        DungenEdge current_edge = outer_polygon[i];
        DungenTriangle t = DungenTriangle(room, current_edge.a, current_edge.b);
        triangulation.push_back(t);
    }

    return false;
}

void DungenPathBuilder::triangulate()
{
    _setup_triangulation();

    clock_t triangulate_start = clock();

    // MAIN LOOP
    for (int room_number = 0; room_number < rooms.size(); room_number++)
    {
        triangulate_point(room_number);
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
    non_spanning_edges.clear();

    if (triangulation.size() == 0)
    {
        return;
    }

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
        if (j >= all_edges.size())
        {
            break;
        }

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
        else if (!non_spanning_edges.has(next_edge))
        {
            non_spanning_edges.push_back(next_edge);
        }

        j++;
    }

    UtilityFunctions::print("MINIMUM SPANNING TREE TIME ELAPSED ", (float)(clock() - start_mst) / CLOCKS_PER_SEC);
}

Vector<DungenEdge> DungenPathBuilder::get_path_edges()
{
    Vector<DungenEdge> path_edges = minimum_spanning_tree.duplicate();

    if (non_spanning_edges.size() == 0)
    {
        return path_edges;
    }

    for (int i = 0; i < Math::max(1, non_spanning_edges.size() / 6); i++)
    {
        path_edges.push_back(non_spanning_edges[i]);
    }

    return path_edges;
}

Vector<Rect2i> DungenPathBuilder::get_path_rectangles()
{
    Vector<Rect2i> path_rectangles;
    Vector<DungenEdge> path_edges = get_path_edges();

    for (int i = 0; i < path_edges.size(); i++)
    {
        DungenEdge current_edge = path_edges[i];
        DungenRoom *room_a = current_edge.a;
        DungenRoom *room_b = current_edge.b;

        Rect2i rect_a = room_a->get_rectangle();
        Rect2i rect_b = room_b->get_rectangle();
        Vector2i edge_center = current_edge.get_center();

        if (
            ((edge_center.x > rect_a.position.x && edge_center.x < rect_a.get_end().x) &&
             (edge_center.x > rect_b.position.x && edge_center.x < rect_b.get_end().x)) /* ||
                                                                                         */
        )
        {
            Vector2i start_position;
            Vector2i end_position;
            if (rect_a.position.y > rect_b.position.y)
            {
                start_position = Vector2i(edge_center.x,
                                          rect_a.position.y);
                end_position = Vector2i(edge_center.x, rect_b.position.y + rect_b.size.y);
            }
            else
            {
                start_position = Vector2i(edge_center.x,
                                          rect_b.position.y);
                end_position = Vector2i(edge_center.x, rect_a.position.y + rect_a.size.y);
            }

            Rect2i rect = Rect2i();
            rect.set_position(start_position);
            rect.set_end(end_position);
            rect = rect
                       .abs()
                       .grow_side(SIDE_RIGHT, 3.0);

            if (rect.has_area())
            {
                path_rectangles.push_back(rect);
            }
        }
        else if ((edge_center.y > rect_a.position.y && edge_center.y < rect_a.get_end().y) &&
                 (edge_center.y > rect_b.position.y && edge_center.y < rect_b.get_end().y))
        {
            Vector2i start_position;
            Vector2i end_position;
            if (rect_a.position.x > rect_b.position.x)
            {
                start_position = Vector2i(rect_a.position.x,
                                          edge_center.y);
                end_position = Vector2i(rect_b.position.x + rect_b.size.x, edge_center.y);
            }
            else
            {
                start_position = Vector2i(rect_b.position.x,
                                          edge_center.y);
                end_position = Vector2i(rect_a.position.x + rect_a.size.x, edge_center.y);
            }

            Rect2i rect = Rect2i();
            rect.set_position(start_position);
            rect.set_end(end_position);
            rect = rect.abs().grow_side(SIDE_BOTTOM, 3.0);

            if (rect.has_area())
            {
                path_rectangles.push_back(rect);
            }
        }
        else
        {
            Vector2i start_position_a = rect_a.get_center();
            Vector2i start_position_b = rect_b.get_center();
            Vector2 end_position = Vector2(rect_a.get_center().x, rect_b.get_center().y);

            Rect2i path_rect_a = Rect2i();

            bool ax_bx = rect_a.get_center().x > rect_b.get_center().x;
            bool ay_by = rect_a.get_center().y > rect_b.get_center().y;

            if (ax_bx)
            {
                //  [b] [a]
                start_position_b = Vector2(rect_b.position.x + rect_b.size.x, rect_b.get_center().y);
                if (ay_by)
                {
                    //  [b]  *
                    //      [a]
                    start_position_a = Vector2(rect_a.get_center().x, rect_a.position.y);
                }
                else
                {
                    //      [a]
                    //  [b]  *
                    start_position_a = Vector2(rect_a.get_center().x, rect_a.position.y + rect_a.size.y);
                }
            }
            else
            {
                // [a] [b]
                start_position_b = Vector2(rect_b.position.x, rect_b.get_center().y);
                if (ay_by)
                {
                    //  *  [b]
                    // [a]
                    start_position_a = Vector2(rect_a.get_center().x, rect_a.position.y);
                }
                else
                {
                    //  [a]
                    //   *  [b]
                    start_position_a = Vector2(rect_a.get_center().x, rect_a.position.y + rect_a.size.y);
                }
            }

            path_rect_a.set_position(start_position_a);
            path_rect_a.set_end(end_position);
            path_rect_a = path_rect_a.abs().grow_side(rect_a.get_center().y > rect_b.get_center().y ? SIDE_RIGHT : SIDE_LEFT, 3.0);
            if (path_rect_a.has_area())
            {
                path_rectangles.push_back(path_rect_a);
            }

            Rect2i path_rect_b = Rect2i();

            // path_rect_b.set_position(rect_b.get_center());
            path_rect_b.set_position(start_position_b);
            path_rect_b.set_end(end_position);
            path_rect_b = path_rect_b.abs().grow_side(rect_a.get_center().x > rect_b.get_center().x ? SIDE_BOTTOM : SIDE_TOP, 3.0);
            if (path_rect_b.has_area())
            {
                path_rectangles.push_back(path_rect_b);
            }
        }
    }

    return path_rectangles;
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