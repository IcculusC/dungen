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

    ADD_SIGNAL(MethodInfo("generation_complete", PropertyInfo(Variant::FLOAT, "time_elapsed")));
}

Dungen::Dungen() : config(Ref<DungenConfig>(memnew(DungenConfig))),
                   all_rooms(Vector<DungenRoom *>()),
                   map_rooms(Vector<DungenRoom *>()),
                   trimmed_rooms(Vector<DungenRoom *>()),
                   rng(RandomNumberGenerator()),
                   total_area(0),
                   path_builder(DungenPathBuilder())
{
    rng.set_seed(config->get_seed());
}

Dungen::~Dungen()
{
    for (int i = 0; i < all_rooms.size(); i++)
    {
        memdelete(all_rooms[i]);
    }
}

void Dungen::set_config(const Ref<DungenConfig> &p_config)
{
    if (config == p_config)
    {
        return;
    }

    config = p_config;
}

Vector<DungenRoom *> Dungen::get_all_rooms() const
{
    return all_rooms;
}

Vector<DungenRoom *> Dungen::get_map() const
{
    return map_rooms;
}

Vector<DungenRoom *> Dungen::get_trimmed_rooms() const
{
    return trimmed_rooms;
}

Dictionary Dungen::get_all() {
    Dictionary results;

    Array room_rects_array;
    for (int i = 0; i < map_rooms.size(); i++) {
        room_rects_array.push_back(map_rooms[i]->get_rectangle());
    }

    Vector<Rect2i> path_rects = path_builder.get_path_rectangles();
    Array path_rects_array;
    for (int i = 0; i < path_rects.size(); i++) {
        path_rects_array.push_back(path_rects[i]);
    }

    results["rooms"] = room_rects_array;
    results["paths"] = path_rects_array;

    return results;
}

void Dungen::generate()
{
    rng.set_seed(config->get_seed());
    double generation_start = (double)clock();
    _generate_rooms();
    _separate_rooms();
    _trim_rooms();

    path_builder.clear_rooms();
    path_builder.add_rooms(map_rooms);
    path_builder.triangulate();
    path_builder.find_minimum_spanning_tree();

    // TODO: generate minimum spanning tree
    // TODO: path rectangles

    emit_signal("generation_complete", ((double)clock() - generation_start) / CLOCKS_PER_SEC);
}

Vector2i Dungen::generate_random_point_in_rectangle(Vector2i &dimensions)
{
    // ASSUMING ORIGIN IS Vector2(0, 0)

    Vector2i half_size = dimensions / 2;

    return Vector2i(
        rng.randi_range(-half_size.x, half_size.x),
        rng.randi_range(-half_size.y, half_size.y));
}

Vector2i Dungen::generate_random_point_in_ellipse(Vector2i &dimensions)
{
    double t = 2 * Math_PI * rng.randf();
    double u = rng.randf() + rng.randf();
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

void Dungen::_reset()
{
    for (int i = 0; i < all_rooms.size(); i++)
    {
        memdelete(all_rooms[i]);
    }
    all_rooms.clear();
    map_rooms.clear();
    trimmed_rooms.clear();
    path_builder.clear_rooms();
    total_area = 0;
}

DungenRoom *Dungen::_generate_room()
{
    DungenShape spawn_area_shape = config->get_spawn_area_shape();
    Vector2i spawn_area_dimensions = config->get_spawn_area_dimensions();

    Vector2i room_dimensions = config->get_room_dimensions();
    Vector2i room_dimensions_sigma = config->get_room_dimensions_sigma();

    Vector2i room_position;
    if (spawn_area_shape == RECTANGLE)
    {
        room_position = generate_random_point_in_rectangle(spawn_area_dimensions);
    }
    else
    {
        room_position = generate_random_point_in_ellipse(spawn_area_dimensions);
    }
    if (room_position == Vector2i(0, 0))
    {
        room_position += Vector2i(rng.randi_range(-5, 5), rng.randi_range(-5, 5));
    }

    double w = Math::abs(rng.randfn(room_dimensions.x, room_dimensions_sigma.x));
    double h = Math::abs(rng.randfn(room_dimensions.y, room_dimensions_sigma.y));
    Vector2i size = Vector2i(w, h);
    Rect2i rect = Rect2i(room_position - (size / 2), size);

    return memnew(DungenRoom(rect));
}

void Dungen::_generate_rooms()
{
    clock_t generate_start = clock();
    _reset();

    int room_count = config->get_room_count();

    for (int i = 0; i < room_count; i++)
    {
        DungenRoom *room = _generate_room();
        total_area += room->get_area();
        all_rooms.push_back(room);
    }

    emit_signal("generate_rooms", ((double)clock() - generate_start) / CLOCKS_PER_SEC);
}

// TODO: implement this maybe optimization of having the ID of the first overlap
int Dungen::_smart_has_overlapping_rooms()
{
    for (int i = 0; i < all_rooms.size(); i += 1)
    {
        DungenRoom *rect_a = all_rooms[i];
        for (int j = 0; j < all_rooms.size(); j += 1)
        {
            if (i == j)
            {
                continue;
            }

            DungenRoom *rect_b = all_rooms[j];
            if (rect_a->intersects(rect_b))
            {
                return i;
            }
        }
    }
    return -1;
}

bool Dungen::_has_overlapping_rooms()
{
    for (int i = 0; i < all_rooms.size(); i += 1)
    {
        DungenRoom *rect_a = all_rooms[i];
        for (int j = 0; j < all_rooms.size(); j += 1)
        {
            if (i == j)
            {
                continue;
            }

            DungenRoom *rect_b = all_rooms[j];
            if (rect_a->intersects(rect_b))
            {
                return true;
            }
        }
    }
    return false;
}

// TODO: make this faster, start at first overlapping room with _smart_has_overlapping_rooms function, test other optimizations
void Dungen::_separate_rooms()
{
    clock_t separate_start = clock();
    while (_has_overlapping_rooms())
    {
        for (int i = 0; i < all_rooms.size(); i += 1)
        {
            Vector2 movement_vector = Vector2(0, 0);
            int neighbors = 0;
            DungenRoom *rect_a = all_rooms[i];
            for (int j = 0; j < all_rooms.size(); j += 1)
            {
                if (i == j)
                {
                    continue;
                }

                DungenRoom *rect_b = all_rooms[j];

                if (!rect_a->intersects(rect_b))
                {
                    continue;
                }

                neighbors++;

                Vector2 center_distance = rect_a->get_center() - rect_b->get_center();

                if (center_distance.is_zero_approx())
                {
                    movement_vector += Vector2(rng.randi_range(-1, 1), rng.randi_range(-1, 1));
                    // neighbors++;
                    // movement_vector = Vector2(rng.randi_range(-5, 5), rng.randi_range(-5, 5));
                    // neighbors = 1;
                    continue;
                }

                // movement_vector += center_distance;
                // neighbors++;
                movement_vector += center_distance;
            }

            if (movement_vector != Vector2i(0, 0) && neighbors > 0)
            {
                Vector2 direction =
                    Vector2(movement_vector / neighbors)
                        .normalized()
                        .snapped(Vector2(1, 1));

                rect_a->set_position(rect_a->get_position() + Vector2i(direction));
            }
        }
    }

    UtilityFunctions::print("SEPARATION TIME ELAPSED ", (float)(clock() - separate_start) / CLOCKS_PER_SEC);
    emit_signal("separate_rooms", ((double)clock() - separate_start) / CLOCKS_PER_SEC);
}

bool Dungen::_should_trim_room(DungenRoom *room, double minimum_area) const
{
    Vector2i room_minimum_dimensions = config->get_room_minimum_dimensions();
    Rect2i rectangle = room->get_rectangle();

    if (
        rectangle.size.x < room_minimum_dimensions.x ||
        rectangle.size.y < room_minimum_dimensions.y ||
        rectangle.get_area() < minimum_area)
    {
        return true;
    }

    return false;
}

// TODO: figure out some less random logic for this because it's quiet unpredictable
void Dungen::_trim_rooms()
{
    double room_dimensions_trim_sigma = config->get_room_dimensions_trim_sigma();

    double average_area = get_average_area();
    double minimum_area = rng.randfn(average_area, room_dimensions_trim_sigma);
    int new_area = 0;

    for (int i = 0; i < all_rooms.size(); i++)
    {
        DungenRoom *rect = all_rooms[i];

        if (_should_trim_room(rect, minimum_area))
        {
            trimmed_rooms.push_back(rect);
            continue;
        }

        rect->set_color(Color::named("POWDERBLUE"));
        map_rooms.push_back(rect);
        new_area += rect->get_area();
    }

    total_area = new_area;
}
