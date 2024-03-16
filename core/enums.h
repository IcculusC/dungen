#ifndef DUNGEN_ENUMS_H
#define DUNGEN_ENUMS_H

#include <godot_cpp/core/class_db.hpp>

namespace godot
{
	enum DungenType
	{
		RANDOM_SHAPE,
		RANDOM_WALK
	};

	enum DungenShape
	{
		ELLIPSE,
		RECTANGLE
	};

}

using namespace godot;

VARIANT_ENUM_CAST(DungenShape)
VARIANT_ENUM_CAST(DungenType)

#endif // DUNGEN_ENUMS_H