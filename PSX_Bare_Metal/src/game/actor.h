#ifndef ACTOR_H
#define ACTOR_H

#include <stdint.h>
#include <model.h>
#include <texture.h>
#include <gte.h>

typedef struct w_actor{
    
    uint32_t type;
    uint32_t state;
    void (*tick)(struct w_actor *self, int last_frame); 
    AST_Allocation *asset;
    GPU_Allocation *texture;
    ANIM_CONTROLLER *animation_controller;
    TRANSFORM actor_transform;
    bool animate;
    uint8_t anim_select;
    uint8_t relative_start_frame;

}ACTOR;

#endif