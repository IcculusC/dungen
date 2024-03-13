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

		void _separation_iteration(int p_start);

		bool _smart_separate_rooms();
		void _separate_rooms();

		bool _should_trim_room(DungenRoom *room, double minimum_area) const;
		void _trim_rooms();

	protected:
		static void _bind_methods();

	public:
		struct Iterator
		{
		private:
			Dungen *dungen;
			int E = -1;
			int stage = 0;

		public:
			_FORCE_INLINE_ int get_stage() const { return stage; }

			_FORCE_INLINE_ int operator*() const
			{
				return E;
			}

			_FORCE_INLINE_ int operator->() const
			{
				return E;
			}

			_FORCE_INLINE_ void next() {
				E++;

				switch (stage)
				{
				case 0:
				{
					if (dungen->all_rooms.size() < dungen->config->get_room_count())
					{
						DungenRoom *room = dungen->_generate_room();
						dungen->total_area += room->get_area();
						dungen->all_rooms.push_back(room);
					}
					else
					{
						stage++;
					}
					break;
				}
				case 1:
				{
					if (dungen->_smart_separate_rooms())
					{
						stage++;
					}
					break;
				}
				case 2:
				{
					dungen->_trim_rooms();
					stage++;
				}
				case 3:
				{
					dungen->path_builder.add_rooms(dungen->map_rooms);
					dungen->path_builder.triangulate();
					stage++;
				}
				case 4:
				{
					dungen->path_builder.find_minimum_spanning_tree();
					stage++;
				}
				default:
					dungen->emit_signal("generation_complete", 0);
					E = -1;
					stage = -1;
				}
			}

			_FORCE_INLINE_ Iterator &operator++()
			{
				E++;

				switch (stage)
				{
				case 0:
				{
					if (dungen->all_rooms.size() < dungen->config->get_room_count())
					{
						DungenRoom *room = dungen->_generate_room();
						dungen->total_area += room->get_area();
						dungen->all_rooms.push_back(room);
					}
					else
					{
						stage++;
					}
					break;
				}
				case 1:
				{
					if (dungen->_smart_separate_rooms())
					{
						stage++;
					}
					break;
				}
				case 2:
				{
					dungen->_trim_rooms();
					stage++;
				}
				case 3:
				{
					dungen->path_builder.add_rooms(dungen->map_rooms);
					dungen->path_builder.triangulate();
					stage++;
				}
				case 4:
				{
					dungen->path_builder.find_minimum_spanning_tree();
					stage++;
				}
				default:
					dungen->emit_signal("generation_complete", 0);
					E = -1;
					stage = -1;
					return *this;
				}

				return *this;
			}

			_FORCE_INLINE_ bool operator==(const Iterator &b) const { return E == b.E && stage == b.stage; }
			_FORCE_INLINE_ bool operator!=(const Iterator &b) const { return E != b.E && stage != b.stage; }

			_FORCE_INLINE_ explicit operator bool() const
			{
				return E != -1;
			}

			_FORCE_INLINE_ Iterator(Dungen *p_dungen, int p_E, int p_stage)
			{
				dungen = p_dungen;
				E = p_E;
				stage = p_stage;
			}
			_FORCE_INLINE_ Iterator() {}
			_FORCE_INLINE_ Iterator(const Iterator &p_it)
			{
				dungen = p_it.dungen;
				E = p_it.E;
				stage = p_it.stage;
			}

			_FORCE_INLINE_ void operator=(const Iterator &p_it)
			{
				dungen = p_it.dungen;
				E = p_it.E;
				stage = p_it.stage;
			}
		};

		_FORCE_INLINE_ Iterator begin()
		{
			_reset();
			return Iterator(this, 0, 0);
		}
		_FORCE_INLINE_ Iterator end()
		{
			return Iterator(this, -1, -1);
		}
		/*
		_FORCE_INLINE_ Iterator last() {
			return Iterator(-1);
		}
		*/




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