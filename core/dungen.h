#ifndef DUNGEN_H
#define DUNGEN_H

#include <godot_cpp/core/math.hpp>

#include <godot_cpp/core/defs.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/timer.hpp>

#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "dungen_config.h"
#include "dungen_path_builder.h"
#include "dungen_room.h"
#include "enums.h"

namespace godot
{

	class Dungen : public Node
	{
		GDCLASS(Dungen, Node)

	private:
		Ref<DungenConfig> config;

		double total_area;

		RandomNumberGenerator rng;

		Vector<DungenRoom *> all_rooms;
		Vector<DungenRoom *> map_rooms;
		Vector<DungenRoom *> trimmed_rooms;

		DungenPathBuilder path_builder;

		Vector2i generate_random_point_in_ellipse(Vector2i &spawn_area_dimensions);
		Vector2i generate_random_point_in_rectangle(Vector2i &spawn_area_dimensions);

		void _reset();

		DungenRoom *_generate_room();
		void _generate_rooms();

		int _smart_has_overlapping_rooms();
		bool _has_overlapping_rooms();

		void _separate_rooms();

		bool _should_trim_room(DungenRoom *room, double minimum_area) const;
		void _trim_rooms();

	protected:
		static void _bind_methods();

	public:
		Dungen();
		~Dungen();

		void generate();

		void set_config(const Ref<DungenConfig> &p_config);
		Ref<DungenConfig> get_config() const { return config; };

		DungenPathBuilder get_path_builder() { return path_builder; }

		double get_average_area() const { return all_rooms.size() > 0 ? total_area / all_rooms.size() : 0; };
		double get_total_area() const { return total_area; };

		Vector<DungenRoom *> get_all_rooms() const;
		Vector<DungenRoom *> get_map() const;
		Vector<DungenRoom *> get_trimmed_rooms() const;

		Dictionary get_all();
	};

}

#endif