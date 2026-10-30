#ifndef F_MATH
#define F_MATH

#include <stdint.h>

#define FIX_SHL(in,n) ((int32_t)(in)*(int32_t)(1<<(n)))
#define FIX_SHR(in,n) ((int32_t)(in)>>(n))

#define CONVERT_A_TO_B(ina,isa,isb) (((isb)>=(isa))?FIX_SHL((ina),(isb)-(isa)):FIX_SHR((ina),(isa)-(isb)))
#define ADD_A_B(ina,inb,isa,isb) (CONVERT_A_TO_B((ina),(isa),(isb))+(int32_t)(inb))
#define SUB_A_B(ina,inb,isa,isb) (CONVERT_A_TO_B((ina),(isa),(isb))-(int32_t)(inb))
#define MULT_A_B(ina,inb,isa,isb) ((int32_t)(((int64_t)(ina)*(int64_t)(inb))>>(isa)))
#define DIV_A_B(ina,inb,isa,isb) ((int32_t)(((int64_t)(ina)*(int64_t)(1<<(isb)))/(int64_t)(inb)))

typedef enum coll_ax_r : uint32_t {
    COLLISION_NONE = 0x0,
    COLLISION_POINT = 0x1,
    COLLISION_BORDER = 0x2,
    COLLISION_OVERLAP = 0x3,
    COLLISION_INSIDE = 0xFFFFFFFF,
    COLLISION_UNDEFINED = 0x80000000,
} COLLISION_AXIS_RESULT;


static inline COLLISION_AXIS_RESULT point_match_test (int32_t a_x, int32_t a_y, int32_t a_z, int32_t b_x, int32_t b_y, int32_t b_z){

    return ((a_x==b_x)&&(a_y==b_y)&&(a_z==b_z))? COLLISION_POINT : COLLISION_NONE;

}


COLLISION_AXIS_RESULT rect_test(int32_t a_north, int32_t a_west, int32_t a_south, int32_t a_east, int32_t b_north, int32_t b_west, int32_t b_south, int32_t b_east , bool self_contained);



#endif