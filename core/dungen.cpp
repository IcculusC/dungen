#include "dungen.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Dungen::_bind_methods()
{
    // generation
    ClassDB::bind_method(D_METHOD("generate"), &Dungen::generate);

    ClassDB::bind_method(D_METHOD("get_all"), &Dungen::get_all);

    ClassDB::bind_method(D_METHOD("get_config"), &Dungen::get_config);
    ClassDB::bind_method(D_METHOD("set_config", "p_config"), &Dungen::set_config);

    ClassDB::add_property("Dungen", PropertyInfo(Variant::OBJECT, "config", PROPERTY_HINT_RESOURCE_TYPE, "DungenConfig", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_EDITOR_INSTANTIATE_OBJECT), "set_config", "get_config");

    ADD_SIGNAL(MethodInfo("generate_rooms", PropertyInfo(Variant::FLOAT, "time_elapsed")));
    ADD_SIGNAL(MethodInfo("separate_rooms", PropertyInfo(Variant::FLOAT, "time_elapsed")));
    ADD_SIGNAL(MethodInfo("triangulate_rooms", PropertyInfo(Variant::FLOAT, "time_elapsed")));

    ADD_SIGNAL(MethodInfo("generation_complete", PropertyInfo(Variant::FLOAT, "time_elapsed")));
}

Dungen::Dungen() : config(Ref<DungenConfig>(memnew(DungenConfig))),
                   rng(Ref<RandomNumberGenerator>(memnew(RandomNumberGenerator))),
                   path_builder(DungenPathBuilder()),
                   room_generator(DungenRoomGenerator(this->config, this->rng))
{
    rng->set_seed(config->get_seed());
}

Dungen::~Dungen()
{
    reset();
}

int Dungen::begin()
{
    reset();
    return current_index;
}

int Dungen::next() {
    switch(phase) {
        case START:
            phase = ROOMS;
            break;
        case ROOMS:
            if (room_generator.next() == -1)
            {
                path_builder.add_rooms(room_generator.get_map_rooms());
                phase = GEOMETRY;
            }
            current_index++;
            break;
        case GEOMETRY:
            if (path_builder.next() == -1)
            {
                phase = COMPLETE;
            }
            current_index++;
            break;
        case COMPLETE:
        default:
            return -1;
    }

    return current_index;
}

void Dungen::set_config(const Ref<DungenConfig> &p_config)
{
    if (config == p_config)
    {
        return;
    }
    room_generator.set_config(p_config);
    config = p_config;
}

Vector<DungenRoom *> Dungen::get_all_rooms() const
{
    return room_generator.get_all_rooms();
}

Vector<DungenRoom *> Dungen::get_map_rooms() const
{
    return room_generator.get_map_rooms();
}

Vector<DungenRoom *> Dungen::get_trimmed_rooms() const
{
    return room_generator.get_trimmed_rooms();
}

Dictionary Dungen::get_all()
{
    Dictionary results;

    Vector<DungenRoom *> map_rooms = room_generator.get_map_rooms();

    Array room_rects_array;
    for (int i = 0; i < map_rooms.size(); i++)
    {
        room_rects_array.push_back(map_rooms[i]->get_rectangle());
    }

    Vector<Rect2i> path_rects = path_builder.get_path_rectangles();
    Array path_rects_array;
    for (int i = 0; i < path_rects.size(); i++)
    {
        path_rects_array.push_back(path_rects[i]);
    }

    results["rooms"] = room_rects_array;
    results["paths"] = path_rects_array;

    return results;
}

void Dungen::generate()
{
    reset();
    double generation_start = (double)clock();

    room_generator.generate();

    path_builder.clear_rooms();
    path_builder.add_rooms(room_generator.get_map_rooms());
    path_builder.triangulate();
    path_builder.find_minimum_spanning_tree();

    /*
    Vector<DungenEdge> path_edges = path_builder.get_path_edges();

    for (DungenEdge &e : path_edges) {
        Vector2 midpoint = Vector2(e.a->get_center().x, e.b->get_center().y);
        Vector2 center = e.get_center();

        for (DungenRoom *e : trimmed_rooms) {
            if (e->get_rectangle().has_point(midpoint) || e->get_rectangle().has_point(center)) {
                e->set_color(Color::named("CORAL"));
                map_rooms.push_back(e);
                trimmed_rooms.erase(e);
            }
        }
    }

    path_builder.clear_rooms();
    path_builder.add_rooms(room_generator.get_map_rooms());
    path_builder.triangulate();
    path_builder.find_minimum_spanning_tree();
    */

    emit_signal("generation_complete", ((double)clock() - generation_start) / CLOCKS_PER_SEC);
}

Vector2i Dungen::generate_random_point_in_rectangle(Vector2i &dimensions)
{
    // ASSUMING ORIGIN IS Vector2(0, 0)

    Vector2i half_size = dimensions / 2;

    return Vector2i(
        rng->randi_range(-half_size.x, half_size.x),
        rng->randi_range(-half_size.y, half_size.y));
}

Vector2i Dungen::generate_random_point_in_ellipse(Vector2i &dimensions)
{
    double t = 2 * Math_PI * rng->randf();
    double u = rng->randf() + rng->randf();
    double r = 0;

    if (u > 1)
    {
        r = 2 - u;
    }
    else
    {
        r = u;
    }

    Vector2 dimensions_d = Vector2(Math::floor(dimensions.x * r * Math::cos(t) / 2), Math::floor(dimensions.y * r * Math::sin(t / 2))).snapped(Vector2i(1, 1));

    return Vector2i(dimensions_d);
}

void Dungen::reset()
{   
    rng->set_seed(config->get_seed());
    path_builder.reset();
    room_generator.reset();

    phase = START;
    current_index = 0;
}
