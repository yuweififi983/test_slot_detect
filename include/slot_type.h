#ifndef _SLOT_TYPE_H_
#define _SLOT_TYPE_H_
#include <cstdint>
#include "type_def.h"
    enum SlotType{
        Vertical = 0,
        Horizontal,
        Tilt,
        Max
    };
    struct SlotInfo{
        l32_point img_pos[4];
        SlotType slot_shape;
        std::uint32_t credit;
    };
#endif
