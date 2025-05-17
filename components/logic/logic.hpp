#ifndef LOGIC_HPP
#define LOGIC_HPP

namespace Logic {
    // Function to map a value from one range to another
    static float remap(float value, float istart, float istop, float ostart, float ostop) { return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));}

}

#endif // LOGIC_HPP