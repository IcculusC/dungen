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

		struct Iterator
		{
		private:
			int E = -1;

		public:
			_FORCE_INLINE_ int &operator*() const
			{
				return E;
			}

			_FORCE_INLINE_ int *operator->() const
			{
				return E;
			}

			_FORCE_INLINE_ Iterator &operator++()
			{
				E++;
				return *this;
			}

			_FORCE_INLINE_ bool operator==(const Iterator &b) const { return E == b.E; }
			_FORCE_INLINE_ bool operator!=(const Iterator &b) const { return E != b.E; }

			_FORCE_INLINE_ explicit operator bool() const
			{
				return E != -1;
			}

			_FORCE_INLINE_ Iterator(int p_E) { E = p_E; }
			_FORCE_INLINE_ Iterator() {}
			_FORCE_INLINE_ Iterator(const Iterator &p_it) { E = p_it.E; }
			
			_FORCE_INLINE_ void operator=(const Iterator &p_it)
			{
				E = p_it.E;
			}
		};
	
	_FORCE_INLINE_ Iterator begin() {
		return Iterator(0);
	}
	_FORCE_INLINE_ Iterator end() {
		return Iterator(-1);
	}
	/*
	_FORCE_INLINE_ Iterator last() {
		return Iterator(-1);
	}
	*/

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