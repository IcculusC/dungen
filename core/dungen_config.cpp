#include "dungen_config.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DungenConfig::set_seed(const double p_seed)
{
    if (seed == p_seed)
    {
        return;
    }

    seed = p_seed;

    emit_changed();
}

void DungenConfig::set_generation_type(const DungenType p_generation_type)
{
    if (generation_type == p_generation_type)
    {
        return;
    }

    generation_type = p_generation_type;

    emit_changed();
}

void DungenConfig::set_room_count(const double p_room_count)
{
    if (room_count == p_room_count)
    {
        return;
    }

    room_count = p_room_count;

    emit_changed();
}

void DungenConfig::set_room_dimensions(const Vector2i p_room_dimensions)
{
    if (room_dimensions == p_room_dimensions)
    {
        return;
    }

    room_dimensions = p_room_dimensions;

    emit_changed();
}

void DungenConfig::set_room_dimensions_sigma(const Vector2i p_room_dimensions_sigma)
{
    if (room_dimensions_sigma == p_room_dimensions_sigma)
    {
        return;
    }

    room_dimensions_sigma = p_room_dimensions_sigma;

    emit_changed();
}

void DungenConfig::set_room_dimensions_trim_ratio(double p_room_dimensions_trim_ratio)
{
    if (room_dimensions_trim_ratio == p_room_dimensions_trim_ratio)
    {
        return;
    }

    room_dimensions_trim_ratio = p_room_dimensions_trim_ratio;

    emit_changed();
}

void DungenConfig::set_room_minimum_dimensions(const Vector2i p_room_minimum_dimensions)
{
    if (room_minimum_dimensions == p_room_minimum_dimensions)
    {
        return;
    }

    room_minimum_dimensions = p_room_minimum_dimensions;

    emit_changed();
}

void DungenConfig::set_spawn_area_dimensions(const Vector2i p_spawn_area_dimensions)
{
    if (spawn_area_dimensions == p_spawn_area_dimensions)
    {
        return;
    }

    spawn_area_dimensions = p_spawn_area_dimensions;

    emit_changed();
}

void DungenConfig::set_spawn_area_shape(const DungenShape p_spawn_area_shape)
{
    if (spawn_area_shape == p_spawn_area_shape)
    {
        return;
    }

    spawn_area_shape = p_spawn_area_shape;

    emit_changed();
}

void DungenConfig::set_step_size(const int p_step_size)
{
    if (step_size == p_step_size)
    {
        return;
    }

    step_size = p_step_size;

    emit_changed();
}

DungenConfig::DungenConfig()
    : seed(-1),
      generation_type(RANDOM_SHAPE),
      room_count(32),
      room_dimensions(Vector2i(10, 10)),
      room_dimensions_sigma(Vector2i(2.0, 2.0)),

      room_dimensions_trim_ratio(1.0),
      room_minimum_dimensions(Vector2i(3, 3)),

      spawn_area_dimensions(Vector2i(10, 10)),
      spawn_area_shape(ELLIPSE),

      step_size(5)
{
}
DungenConfig::~DungenConfig() {}

void DungenConfig::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_seed"), &DungenConfig::get_seed);
    ClassDB::bind_method(D_METHOD("set_seed", "p_seed"), &DungenConfig::set_seed);

    ClassDB::bind_method(D_METHOD("get_generation_type"), &DungenConfig::get_generation_type);
    ClassDB::bind_method(D_METHOD("set_generation_type", "p_generation_type"), &DungenConfig::set_generation_type);

    ClassDB::bind_method(D_METHOD("get_room_count"), &DungenConfig::get_room_count);
    ClassDB::bind_method(D_METHOD("set_room_count", "p_room_count"), &DungenConfig::set_room_count);

    ClassDB::bind_method(D_METHOD("get_room_dimensions"), &DungenConfig::get_room_dimensions);
    ClassDB::bind_method(D_METHOD("set_room_dimensions", "p_room_dimensions"), &DungenConfig::set_room_dimensions);

    ClassDB::bind_method(D_METHOD("get_room_dimensions_sigma"), &DungenConfig::get_room_dimensions_sigma);
    ClassDB::bind_method(D_METHOD("set_room_dimensions_sigma", "p_room_dimensions_sigma"), &DungenConfig::set_room_dimensions_sigma);

    ClassDB::bind_method(D_METHOD("get_room_dimensions_trim_ratio"), &DungenConfig::get_room_dimensions_trim_ratio);
    ClassDB::bind_method(D_METHOD("set_room_dimensions_trim_ratio", "p_room_dimensions_trim_ratio"), &DungenConfig::set_room_dimensions_trim_ratio);

    ClassDB::bind_method(D_METHOD("get_room_minimum_dimensions"), &DungenConfig::get_room_minimum_dimensions);
    ClassDB::bind_method(D_METHOD("set_room_minimum_dimensions", "p_room_minimum_dimensions"), &DungenConfig::set_room_minimum_dimensions);

    ClassDB::bind_method(D_METHOD("get_spawn_area_dimensions"), &DungenConfig::get_spawn_area_dimensions);
    ClassDB::bind_method(D_METHOD("set_spawn_area_dimensions", "p_spawn_area_dimensions"), &DungenConfig::set_spawn_area_dimensions);

    ClassDB::bind_method(D_METHOD("get_spawn_area_shape"), &DungenConfig::get_spawn_area_shape);
    ClassDB::bind_method(D_METHOD("set_spawn_area_shape", "p_spawn_area_shape"), &DungenConfig::set_spawn_area_shape);

    ClassDB::bind_method(D_METHOD("get_step_size"), &DungenConfig::get_step_size);
    ClassDB::bind_method(D_METHOD("set_step_size", "p_set_step_size"), &DungenConfig::set_step_size);

    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::FLOAT, "seed"), "set_seed", "get_seed");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::INT, "generation_type", PROPERTY_HINT_ENUM, "RANDOM_SHAPE,RANDOM_WALK"), "set_generation_type", "get_generation_type");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::INT, "room_count"), "set_room_count", "get_room_count");

    ADD_GROUP("Common", "");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::VECTOR2I, "room_dimensions"), "set_room_dimensions", "get_room_dimensions");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::VECTOR2I, "room_dimensions_sigma"), "set_room_dimensions_sigma", "get_room_dimensions_sigma");

    ADD_GROUP("Random Shape", "");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::INT, "spawn_area_shape", PROPERTY_HINT_ENUM, "ELLIPSE,RECTANGLE"), "set_spawn_area_shape", "get_spawn_area_shape");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::VECTOR2I, "spawn_area_dimensions"), "set_spawn_area_dimensions", "get_spawn_area_dimensions");

    ADD_GROUP("Random Walk", "");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::INT, "step_size"), "set_step_size", "get_step_size");

    ADD_GROUP("Room Trimming", "");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::FLOAT, "room_dimensions_trim_ratio", PROPERTY_HINT_RANGE, "0.0,2.0"), "set_room_dimensions_trim_ratio", "get_room_dimensions_trim_ratio");
    ClassDB::add_property("DungenConfig", PropertyInfo(Variant::VECTOR2I, "room_minimum_dimensions"), "set_room_minimum_dimensions", "get_room_minimum_dimensions");

    ADD_SIGNAL(MethodInfo("changed"));
}
