#include <fxmath.h>

static inline bool collision_touch(COLLISION_AXIS_RESULT r){
    return (r==COLLISION_POINT)||(r==COLLISION_BORDER);
}

static inline COLLISION_AXIS_RESULT rect_aabb_test (int32_t a_min, int32_t a_max, int32_t b_min, int32_t b_max){

    if(a_max>=b_min){

        if(a_min>=b_max){
            return (a_min==b_max)? COLLISION_POINT : COLLISION_NONE;
        }

        return(a_max==b_min)? COLLISION_POINT : COLLISION_BORDER;

    }

    return COLLISION_NONE;

}

static inline COLLISION_AXIS_RESULT rect_side_test(int32_t a_north, int32_t a_west, int32_t a_east, int32_t b_north, int32_t b_west, int32_t b_south, int32_t b_east){

    if(a_north==b_south){

        return rect_aabb_test(a_west,a_east,b_west,b_east);
       
    }
    else if((a_north>b_south)&&(a_north<=b_north)){
        if((a_west<b_east)&&(b_west<a_east)){

            return COLLISION_OVERLAP;

        }
    }

    return COLLISION_NONE;
}


COLLISION_AXIS_RESULT rect_test(int32_t a_north, int32_t a_west, int32_t a_south, int32_t a_east, int32_t b_north, int32_t b_west, int32_t b_south, int32_t b_east, bool self_contained){

    if((a_north < a_south) || (a_east < a_west) || (b_north < b_south) || (b_east < b_west)){
        return COLLISION_UNDEFINED;
    }

    if(!self_contained){

        if((a_north == a_south) || (a_east == a_west) || (b_north == b_south) || (b_east == b_west)){
            return COLLISION_UNDEFINED;
        }
    }

    if(((b_west<=a_west)&&(a_east<=b_east)&&(b_south<=a_south)&&(a_north<=b_north))||((a_west<=b_west)&&(b_east<=a_east)&&(a_south<=b_south)&&(b_north<=a_north))){
        return COLLISION_INSIDE;
    }

    COLLISION_AXIS_RESULT north = rect_side_test(a_north,a_west,a_east,b_north,b_west,b_south,b_east);
    COLLISION_AXIS_RESULT west  = rect_side_test(-a_west,a_south,a_north,-b_west,b_south,-b_east,b_north);
    COLLISION_AXIS_RESULT south = rect_side_test(-a_south,a_west,a_east,-b_south,b_west,-b_north,b_east);
    COLLISION_AXIS_RESULT east  = rect_side_test(a_east,a_south,a_north,b_east,b_south,b_west,b_north);

    return (north<<6)|(west<<4)|(south<<2)|east;
}
