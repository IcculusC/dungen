#ifdef TOOLS_ENABLED

#ifndef DUNGEN_PREVIEWER_H
#define DUNGEN_PREVIEWER_H

#include <godot_cpp/classes/control.hpp>

#include "../core/dungen.h"

namespace godot
{

    class DungenPreviewer : public Control
    {
        GDCLASS(DungenPreviewer, Control)

    private:
        Dungen *dungen_instance;

    protected:
        static void _bind_methods();

    public:
        DungenPreviewer();
        ~DungenPreviewer();

        void set_dungen_instance(Dungen *dungen);

        virtual void _draw() override;
    };

}

#endif

#endif // TOOLS_ENABLED