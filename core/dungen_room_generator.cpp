#include "dungen_room_generator.h"

using namespace godot;

DungenRoomGenerator::DungenRoomGenerator(){};
DungenRoomGenerator::~DungenRoomGenerator(){};

int DungenRoomGenerator::begin()
{
    _reset();
    return current_step;
}

int DungenRoomGenerator::next(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng)
{
    switch (phase)
    {
    case START:
        phase = GENERATE;
        current_step++;
        // can fall down just need a starting and ending state
    case GENERATE:
        if (all_rooms.size() < config->get_room_count())
        {
            DungenRoom *room = _generate_room(config, rng);
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
            _smart_separation_iteration(first_overlap, rng);
        }
        else
        {
            phase = TRIM;
        }
        current_step++;
        break;
    }
    case TRIM:
        _smart_trim_rooms(config, rng);
        phase = COMPLETE;
        break;
    case COMPLETE:
    default:
        return -1;
    }

    return current_step;
}

void DungenRoomGenerator::_reset()
{
    for (int i = 0; i < all_rooms.size(); i++)
    {
        memdelete(all_rooms[i]);
    }
    all_rooms.clear();
    map_rooms.clear();
    trimmed_rooms.clear();

    current_step = 0;

    map_area = 0;
    total_area = 0;
    trimmed_area = 0;

    phase = START;
}

Vector2i DungenRoomGenerator::generate_random_point_in_rectangle(Vector2i &dimensions, Ref<RandomNumberGenerator> &rng)
{
    Vector2i half_size = dimensions / 2;

    return Vector2i(
        rng->randi_range(-half_size.x, half_size.x),
        rng->randi_range(-half_size.y, half_size.y));
}

Vector2i DungenRoomGenerator::generate_random_point_in_ellipse(Vector2i &dimensions, Ref<RandomNumberGenerator> &rng)
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

DungenRoom *DungenRoomGenerator::_generate_room(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng)
{
    DungenShape spawn_area_shape = config->get_spawn_area_shape();
    Vector2i spawn_area_dimensions = config->get_spawn_area_dimensions();

    Vector2i room_dimensions = config->get_room_dimensions();
    Vector2i room_dimensions_sigma = config->get_room_dimensions_sigma();

    Vector2i room_position;
    if (spawn_area_shape == RECTANGLE)
    {
        room_position = generate_random_point_in_rectangle(spawn_area_dimensions, rng);
    }
    else
    {
        room_position = generate_random_point_in_ellipse(spawn_area_dimensions, rng);
    }

    double w = Math::abs(rng->randfn(room_dimensions.x, room_dimensions_sigma.x));
    double h = Math::abs(rng->randfn(room_dimensions.y, room_dimensions_sigma.y));
    Vector2i size = Vector2i(w, h);
    Rect2i rect = Rect2i(room_position - (size / 2), size);

    return memnew(DungenRoom(rect));
}

void DungenRoomGenerator::_generate_rooms(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng)
{
    _reset();

    int room_count = config->get_room_count();

    for (int i = 0; i < room_count; i++)
    {
        DungenRoom *room = _generate_room(config, rng);
        total_area += room->get_area();
        all_rooms.push_back(room);
    }
}

int DungenRoomGenerator::_smart_has_overlapping_rooms()
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

bool DungenRoomGenerator::_smart_separate_rooms(Ref<RandomNumberGenerator> &rng)
{
    int first_index = _smart_has_overlapping_rooms();

    if (first_index == -1)
    {
        return true;
    }

    _smart_separation_iteration(first_index, rng);

    return false;
}

void DungenRoomGenerator::_smart_separation_iteration(int starting_room, Ref<RandomNumberGenerator> &rng)
{
    for (int i = starting_room; i < all_rooms.size(); i += 1)
    {
        Vector2 movement_vector = Vector2(0, 0);
        int neighbors = 0;
        DungenRoom *rect_a = all_rooms[i];

        // i realize waht this says but they jiggle a lot if you don't lol
        for (int j = starting_room; j < all_rooms.size(); j++)
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
                movement_vector += Vector2(rng->randi_range(-1, 1), rng->randi_range(-1, 1));
                // neighbors++;
                // movement_vector = Vector2(rng->randi_range(-5, 5), rng->randi_range(-5, 5));
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

bool DungenRoomGenerator::_smart_should_trim_room(DungenRoom *room, Ref<DungenConfig> &config, double minimum_area) const
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

void DungenRoomGenerator::_smart_trim_rooms(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng)
{
    double room_dimensions_trim_sigma = config->get_room_dimensions_trim_sigma();

    double average_area = get_average_area();
    double minimum_area = rng->randfn(average_area, room_dimensions_trim_sigma);
    map_area = 0;
    trimmed_area = 0;

    for (int i = 0; i < all_rooms.size(); i++)
    {
        DungenRoom *rect = all_rooms[i];

        if (_smart_should_trim_room(rect, config, minimum_area))
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
