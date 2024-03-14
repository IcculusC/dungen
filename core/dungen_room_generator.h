#ifndef DUNGEN_ROOM_GENERATOR_H
#define DUNGEN_ROOM_GENERATOR_H

#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ref.hpp>

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


namespace godot {
    class DungenRoomGenerator {
        enum Phase {
            START,
            GENERATE,
            SEPARATE,
            TRIM,
            COMPLETE
        } phase;

        int current_step;

        int map_area;
        int total_area;
        int trimmed_area;

        Vector<DungenRoom *> all_rooms;
		Vector<DungenRoom *> map_rooms;
		Vector<DungenRoom *> trimmed_rooms;

        Vector2i generate_random_point_in_ellipse(Vector2i &spawn_area_dimensions, Ref<RandomNumberGenerator> &rng);
		Vector2i generate_random_point_in_rectangle(Vector2i &spawn_area_dimensions, Ref<RandomNumberGenerator> &rng);

		DungenRoom *_generate_room(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng);
		void _generate_rooms(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng);

		int _smart_has_overlapping_rooms();

		bool _smart_separate_rooms(Ref<RandomNumberGenerator> &rng);
		void _smart_separation_iteration(int starting_room, Ref<RandomNumberGenerator> &rng);

		bool _smart_should_trim_room(DungenRoom *room, Ref<DungenConfig> &config, double minimum_area) const;
        void _smart_trim_rooms(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng);

        void _reset();

    public:
        DungenRoomGenerator();
        ~DungenRoomGenerator();

        void generate();

        double get_average_area() const { return all_rooms.size() > 0 ? total_area / all_rooms.size() : 0; };
		double get_total_area() const { return total_area; };

        int begin();
        int end() { return -1; }
        int next(Ref<DungenConfig> &config, Ref<RandomNumberGenerator> &rng);
    };
}

#endif // DUNGEN_ROOM_GENERATOR_H