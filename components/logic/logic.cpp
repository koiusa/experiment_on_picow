#include "logic.h"

namespace logic {
    // Function to map a value from one range to another
    float remap(float value, float istart, float istop, float ostart, float ostop) { return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));}

}
