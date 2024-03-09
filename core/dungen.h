#ifndef DUNGEN_H
#define DUNGEN_H

#include <godot_cpp/core/math.hpp>

#include <godot_cpp/core/defs.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/timer.hpp>

#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "dungen_config.h"
#include "dungen_room.h"
#include "enums.h"

namespace godot {

class Dungen : public Node2D {
	GDCLASS(Dungen, Node2D)

private:
	Ref<DungenConfig> config;

	double total_area;

	RandomNumberGenerator rng;
	
	Vector<Ref<DungenRoom>> all_rooms;
	Vector<Ref<DungenRoom>> map_rooms;
	Vector<Ref<DungenRoom>> trimmed_rooms;

	Vector2i generate_random_point_in_ellipse(Vector2i &spawn_area_dimensions);
	Vector2i generate_random_point_in_rectangle(Vector2i &spawn_area_dimensions);

	void _reset();
	Ref<DungenRoom> _generate_room();
	void _generate_rooms();
	int _smart_has_overlapping_rooms();
	bool _has_overlapping_rooms();
	void _separate_rooms();
	void _trim_rooms();


protected:
	static void _bind_methods();

public:
	Dungen();
	~Dungen();

	void generate();

	void set_config(const Ref<DungenConfig> &p_config);
	Ref<DungenConfig> get_config() const { return config; };

	double get_average_area() const { return all_rooms.size() > 0 ? total_area / all_rooms.size() : 0; };
	double get_total_area() const { return total_area; };

	Array get_all_rooms() const;
	Array get_map() const;
	Array get_trimmed_rooms() const;
};

}

#endif