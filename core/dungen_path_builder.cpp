#include "dungen_path_builder.h"

using namespace godot;

DungenPathBuilder::DungenPathBuilder() {}
DungenPathBuilder::~DungenPathBuilder() {}

void DungenPathBuilder::add_rooms(const Vector<Ref<DungenRoom>> &p_rooms) {
    rooms.append_array(p_rooms);
}

void DungenPathBuilder::add_room(const Ref<DungenRoom> &p_room) {
    rooms.push_back(p_room);
}

void DungenPathBuilder::clear_rooms() {
    rooms.clear();
}

void DungenPathBuilder::triangulate() {
    triangulation.clear();
    corners.clear();
    
    clock_t triangulate_start = clock();
    // CREATE INITIAL GEOMETRY
    super_rect = Rect2(-1, -1, 1, 1);
    for (int i = 0; i < rooms.size(); i++) {
        super_rect.expand_to(rooms[i]->get_center());
    }

    super_rect.grow_by(Math::max(super_rect.size.x, super_rect.size.y) / 2);
    
    Vector2 corner0 = super_rect.position;
    Vector2 corner1 = super_rect.position + (Vector2(1, 0) * super_rect.size);
    Vector2 corner2 = super_rect.position + super_rect.size;
    Vector2 corner3 = super_rect.position + (Vector2(0, 1) * super_rect.size);

    corners.push_back(memnew(DungenRoom(corner0)));
    corners.push_back(memnew(DungenRoom(corner1)));
    corners.push_back(memnew(DungenRoom(corner2)));
    corners.push_back(memnew(DungenRoom(corner3)));

    triangulation.push_back(DungenTriangle(corners[0], corners[1], corners[2]));
    triangulation.push_back(DungenTriangle(corners[2], corners[3], corners[0]));
    // CREATE INITIAL GEOMETRY

    // MAIN LOOP
    for (int room_number = 0; room_number < rooms.size(); room_number++) {
        const Ref<DungenRoom> room = rooms[room_number];

        Vector<DungenTriangle> bad_triangles;

        // find triangles whos circumcircle contains the rooms center
        for (int i = 0; i < triangulation.size(); i++) {
            DungenTriangle current_triangle = triangulation[i];

            if (current_triangle.is_point_in_circumfrence(room->get_center())) {
                bad_triangles.push_back(current_triangle);
            }
        }

        // delete the bad ones from the overall triangulation
        for (int i = 0; i < bad_triangles.size(); i++) {
            triangulation.erase(bad_triangles[i]);
        }
    
        Vector<DungenEdge> bad_edges;
        Vector<DungenEdge> outer_polygon;

        for (int i = 0; i < bad_triangles.size(); i++) {
            outer_polygon.push_back(bad_triangles[i].ab);
            outer_polygon.push_back(bad_triangles[i].bc);
            outer_polygon.push_back(bad_triangles[i].ca);
        }

        for (int i = 0; i < outer_polygon.size(); i++) {
            for (int j = 0; j < outer_polygon.size(); j++) {
                if (i == j) {
                    continue;
                }
                if (outer_polygon[i] == outer_polygon[j]) {
                    bad_edges.push_back(outer_polygon[i]);
                    bad_edges.push_back(outer_polygon[j]);
                }
            }
        }

        for (int i = 0; i < bad_edges.size(); i++) {
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

        for (int i = 0; i < outer_polygon.size(); i++) {
            DungenEdge current_edge = outer_polygon[i];
            DungenTriangle t = DungenTriangle(room, current_edge.a, current_edge.b);
            triangulation.push_back(t);
        }
    }

    /*
    Vector<DungenTriangle> corner_triangles;
    
    for (int t = 0; t < triangulation.size(); t++) {
        DungenTriangle current_triangle = triangulation[t];
        
        if (corners.has(current_triangle.a) || corners.has(current_triangle.b) || corners.has(current_triangle.c)) {
            corner_triangles.push_back(current_triangle);
        }
    }

    for (int t = 0; t < corner_triangles.size(); t++) {
        triangulation.erase(corner_triangles[t]);
    }
    */
    // MAIN LOOP

    UtilityFunctions::print("TRIANGULATION TIME ELAPSED ", (float)(clock() - triangulate_start) / CLOCKS_PER_SEC);
}
