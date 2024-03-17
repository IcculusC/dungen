#include "dungen_room_generator.h"

using namespace godot;

DungenRoomGenerator::DungenRoomGenerator(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng)
    : config(config), rng(rng)
{
}
DungenRoomGenerator::~DungenRoomGenerator() {
    reset();
}

void DungenRoomGenerator::set_config(const Ref<DungenConfig> &p_config)
{
    if (config == p_config)
    {
        return;
    }
    config = p_config;
    reset();
}

void DungenRoomGenerator::generate()
{
    reset();
    while (this->next() != this->end())
    {
    }
}

int DungenRoomGenerator::begin()
{
    reset();
    return current_step;
}

int DungenRoomGenerator::next()
{
    switch (phase)
    {
    case START:
        phase = GENERATE;
        current_step++;
        break;
    case GENERATE:
        if (all_rooms.size() < config->get_room_count())
        {
            DungenRoom *room = _generate_room();
            total_area += room->get_area();
            all_rooms.push_back(room);
        }
        else
        {
            phase = SEPARATE;
        }
        current_step++;
        break;
    case SEPARATE:
    {
        int first_overlap = _smart_has_overlapping_rooms();
        if (first_overlap != -1)
        {
            _smart_separation_iteration(first_overlap);
        }
        else
        {
            phase = TRIM;
        }
        current_step++;
        break;
    }
    case TRIM:
        _smart_trim_rooms();
        phase = COMPLETE;
        current_step++;
        break;
    case COMPLETE:
    default:
        return -1;
    }

    return current_step;
}

void DungenRoomGenerator::reset()
{
    for (int i = 0; i < all_rooms.size(); i++)
    {
        memdelete(all_rooms[i]);
    }

    all_rooms.clear();
    map_rooms.clear();
    trimmed_rooms.clear();

    map_area = 0;
    total_area = 0;
    trimmed_area = 0;

    phase = START;
    current_step = 0;
    current_walk_position = Vector2(0, 0);
}

Vector2i DungenRoomGenerator::generate_random_point_in_rectangle(const Vector2i &dimensions)
{
    Vector2i half_size = dimensions / 2;

    return Vector2i(
        rng->randi_range(-half_size.x, half_size.x),
        rng->randi_range(-half_size.y, half_size.y));
}

Vector2i DungenRoomGenerator::generate_random_point_in_ellipse(const Vector2i &dimensions)
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

DungenRoom *DungenRoomGenerator::_generate_room()
{
    DungenType generation_type = config->get_generation_type();
    DungenShape spawn_area_shape = config->get_spawn_area_shape();
    Vector2i spawn_area_dimensions = config->get_spawn_area_dimensions();

    Vector2i room_dimensions = config->get_room_dimensions();
    Vector2i room_dimensions_sigma = config->get_room_dimensions_sigma();

    Vector2i room_position = Vector2i(0, 0);
    switch (generation_type)
    {
    case RANDOM_SHAPE:

        if (spawn_area_shape == RECTANGLE)
        {
            room_position = generate_random_point_in_rectangle(spawn_area_dimensions);
        }
        else
        {
            room_position = generate_random_point_in_ellipse(spawn_area_dimensions);
        }
        break;
    case RANDOM_WALK:
        Vector2i directions[8] = {
            Vector2i(1, 0),
            Vector2i(0, 1),
            Vector2i(1, 1),
            Vector2i(-1, 0),
            Vector2i(0, -1),
            Vector2i(-1, -1),
            Vector2i(-1, 1),
            Vector2i(1, -1)};
        int direction = rng->randi_range(0, 7);
        current_walk_position += directions[direction] * config->get_step_size(); 
        room_position = current_walk_position;
        break;
    }

    double w = Math::abs(rng->randfn(room_dimensions.x, room_dimensions_sigma.x));
    double h = Math::abs(rng->randfn(room_dimensions.y, room_dimensions_sigma.y));
    Vector2i size = Vector2i(w, h);
    Rect2i rect = Rect2i(room_position - (size / 2), size);

    return memnew(DungenRoom(rect));
}

void DungenRoomGenerator::_generate_rooms()
{
    reset();

    int room_count = config->get_room_count();

    for (int i = 0; i < room_count; i++)
    {
        DungenRoom *room = _generate_room();
        total_area += room->get_area();
        all_rooms.push_back(room);
    }
}

int DungenRoomGenerator::_smart_has_overlapping_rooms()
{
    for (int i = 0; i < all_rooms.size(); i += 1)
    {
        DungenRoom *room_a = all_rooms[i];
        for (int j = 0; j < all_rooms.size(); j += 1)
        {
            if (i == j)
            {
                continue;
            }

            DungenRoom *room_b = all_rooms[j];
            if (room_a->intersects(room_b))
            {
                return i;
            }
        }
    }
    return -1;
}

bool DungenRoomGenerator::_smart_separate_rooms()
{
    int first_index = _smart_has_overlapping_rooms();

    if (first_index == -1)
    {
        return true;
    }

    _smart_separation_iteration(first_index);

    return false;
}

void DungenRoomGenerator::_smart_separation_iteration(int starting_room)
{
    for (int i = starting_room; i < all_rooms.size(); i += 1)
    {
        Vector2 movement_vector = Vector2(0, 0);
        int neighbors = 0;
        DungenRoom *room_a = all_rooms[i];

        // i realize waht this says given i == j but they jiggle a lot if you don't lol
        for (int j = starting_room; j < all_rooms.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            DungenRoom *room_b = all_rooms[j];

            if (!room_a->intersects(room_b))
            {
                continue;
            }

            neighbors++;

            Vector2 center_distance = room_a->get_center() - room_b->get_center();

            if (center_distance.is_zero_approx())
            {
                movement_vector += Vector2(rng->randi_range(-1, 1), rng->randi_range(-1, 1));
                continue;
            }

            movement_vector += center_distance;
        }

        if (movement_vector != Vector2i(0, 0) && neighbors > 0)
        {
            Vector2 direction =
                Vector2(movement_vector / neighbors)
                    .normalized()
                    .snapped(Vector2(1, 1));

            room_a->set_position(room_a->get_position() + Vector2i(direction));
        }
    }
}

bool DungenRoomGenerator::_smart_should_trim_room(DungenRoom *room, double minimum_area) const
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

bool DungenRoomGenerator::_smart_trim_one()
{
    // TODO: figure this out later
    /*
    double room_dimensions_trim_ratio = config->get_room_dimensions_trim_ratio();
    double average_area = get_average_area();
    double minimum_area = average_area * room_dimensions_trim_ratio;

    Vector<DungenRoom *> keepers;
    int kept_area;
    for (int i = 0; i < all_rooms.size(); i++)
    {
        DungenRoom *rect = all_rooms[i];

        if (_smart_should_trim_room(rect, minimum_area))
        {
            trimmed_rooms.push_back(rect);
            trimmed_area += rect->get_area();
            return true;
        }

        keepers.push_back(rect);
        kept_area += rect->get_area();
    }

    map_rooms.append_array(keepers);
    map_area = kept_area;

    return false;
    */
    return false;
}

void DungenRoomGenerator::_smart_trim_rooms()
{
    double room_dimensions_trim_ratio = config->get_room_dimensions_trim_ratio();

    double average_area = get_average_area();
    double minimum_area = average_area * room_dimensions_trim_ratio;
    map_area = 0;
    trimmed_area = 0;

    for (int i = 0; i < all_rooms.size(); i++)
    {
        DungenRoom *rect = all_rooms[i];

        if (_smart_should_trim_room(rect, minimum_area))
        {
            trimmed_rooms.push_back(rect);
            trimmed_area += rect->get_area();
            continue;
        }

        rect->set_color(Color::named("POWDERBLUE"));
        map_rooms.push_back(rect);
        map_area += rect->get_area();
    }
}
