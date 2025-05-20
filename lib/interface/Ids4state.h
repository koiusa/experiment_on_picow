#ifndef IDS4STATE_H
#define IDS4STATE_H

#include "struct_ds4_on_pico_w.hpp"

class IDs4state {
public:
    virtual void attach(const DualShock4_state* state) = 0;
protected:
    const DualShock4_state* state = {0};
};

#endif // IDS4STATE_H
