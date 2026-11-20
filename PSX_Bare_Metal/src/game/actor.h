#ifndef ACTOR_T
#define ACTOR_T


#include <scene.h>

typedef enum actor_type : uint32_t{
    A_TYP_EMPTY = 0,
    A_TYP_PLAYER = 1,
    A_TYP_RAVEN = 2,
    A_TYP_SKELETON = 3,
    A_TYP_MINION = 4
}ACTOR_TYPE;


ACTOR *create_specific_actor(uint32_t type);



#endif