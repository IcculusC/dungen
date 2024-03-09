#ifndef DUNGEN_CONFIG_H
#define DUNGEN_CONFIG_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "enums.h"

namespace godot
{

	class DungenConfig : public Resource
	{
		GDCLASS(DungenConfig, Resource)

	private:
		double seed;
		double room_count;

		Vector2i room_dimensions;
		Vector2i room_dimensions_sigma;

		double room_dimensions_trim_sigma;
		Vector2i room_minimum_dimensions;

		Vector2i spawn_area_dimensions;
		DungenShape spawn_area_shape;

		void emit_changed();

	protected:
		static void _bind_methods();

	public:
		DungenConfig();
		~DungenConfig();

		// basic generation stuff
		void set_seed(const double p_seed);
		double get_seed() const { return seed; }

		// room stuff
		void set_room_count(const double p_room_count);
		double get_room_count() const { return room_count; }

		void set_room_dimensions(const Vector2i p_room_dimensions);
		Vector2i get_room_dimensions() const { return room_dimensions; }

		void set_room_dimensions_sigma(const Vector2i p_room_dimensions_sigma);
		Vector2i get_room_dimensions_sigma() const { return room_dimensions_sigma; }

		void set_room_dimensions_trim_sigma(const double p_room_dimensions_trim_sigma);
		double get_room_dimensions_trim_sigma() const { return room_dimensions_trim_sigma; }

		void set_room_minimum_dimensions(const Vector2i p_room_minimum_dimensions);
		Vector2i get_room_minimum_dimensions() const { return room_minimum_dimensions; }

		// spawn area stuff
		void set_spawn_area_dimensions(const Vector2i p_spawn_area_dimensions);
		Vector2i get_spawn_area_dimensions() const { return spawn_area_dimensions; };

		void set_spawn_area_shape(const DungenShape p_spawn_area_shape);
		DungenShape get_spawn_area_shape() const { return spawn_area_shape; };
	};

}

#endif // DUNGEN_CONFIG_H