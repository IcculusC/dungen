#ifndef DUNGEN_ROOM_GENERATOR_H
#define DUNGEN_ROOM_GENERATOR_H

#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "dungen_config.h"
#include "dungen_room.h"
#include "enums.h"

namespace godot
{
    class DungenRoomGenerator
    {
    private:
        enum Phase
        {
            START,
            GENERATE,
            SEPARATE,
            TRIM,
            COMPLETE
        } phase;

        Ref<DungenConfig> config;
        Ref<RandomNumberGenerator> rng;

        int current_step;
        Vector2 current_walk_position;

        int map_area;
        int total_area;
        int trimmed_area;

        Vector<DungenRoom *> all_rooms;
        Vector<DungenRoom *> map_rooms;
        Vector<DungenRoom *> trimmed_rooms;

        Vector2i generate_random_point_in_ellipse(Vector2i &spawn_area_dimensions);
        Vector2i generate_random_point_in_rectangle(Vector2i &spawn_area_dimensions);

        DungenRoom *_generate_room();
        void _generate_rooms();

        int _smart_has_overlapping_rooms();

        bool _smart_separate_rooms();
        void _smart_separation_iteration(int starting_room);

        bool _smart_should_trim_room(DungenRoom *room, double minimum_area) const;
        bool _smart_trim_one();
        void _smart_trim_rooms();

    public:
        DungenRoomGenerator(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng);
        ~DungenRoomGenerator();

        void set_config(const Ref<DungenConfig> &p_config);

        void generate();

        double get_average_area() const { return all_rooms.size() > 0 ? 1.0 * total_area / all_rooms.size() : 0.0; };
        int get_map_area() const { return map_area; };
        int get_total_area() const { return total_area; };
        int get_trimmed_area() const { return trimmed_area; };

        int begin();
        int end() { return -1; }
        int next();

        void reset();

        const Vector<DungenRoom *> &get_all_rooms() const { return all_rooms; };
        const Vector<DungenRoom *> &get_map_rooms() const { return map_rooms; };
        const Vector<DungenRoom *> &get_trimmed_rooms() const { return trimmed_rooms; };
    };
}

#endif // DUNGEN_ROOM_GENERATOR_H