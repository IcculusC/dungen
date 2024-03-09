#ifndef DUNGEN_ENUMS_H
#define DUNGEN_ENUMS_H

#include <godot_cpp/core/class_db.hpp>

namespace godot {

enum DungenShape {
	ELLIPSE,
	RECTANGLE
};

}

using namespace godot;

VARIANT_ENUM_CAST(DungenShape)

#endif // DUNGEN_ENUMS_H