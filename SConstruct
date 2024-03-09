#!/usr/bin/env python
import os
import sys

env = SConscript("extern/godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["."])
sources = Glob("./*.cpp")
sources += Glob("./editor/*.cpp")
sources += Glob("./core/*.cpp")

for s in sources:
    print(s)

print("./demo/bin/libdungen.{}.{}.framework/libdungen.{}.{}".format(
    env["platform"], env["target"], env["platform"], env["target"]
))

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "./demo/bin/libdungen.{}.{}.framework/libdungen.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "./demo/bin/libdungen{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
