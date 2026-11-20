#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>
#include <gte.h>
#include <region.h>

#include <scene.h>
#include <stdlib.h>
#include <stdio.h>
#include <fxmath.h>
#include <hardware.h>

#include <audio.h>
#include <texture.h>
#include <model.h>

#define MAX_ACTORS 16

typedef struct w_actor{
    
    uint32_t type;
    uint32_t state;
    void (*tick)(struct w_actor *self, int last_frame); 
    AST_Allocation *asset;
    GPU_Allocation *texture;
    TRANSFORM actor_transform;

}ACTOR;

void vblank_irq(void);

int get_frame(void);

extern volatile bool second_framebuffer;

extern volatile bool release;

void setup(void);

void tick(int last_frame);

ACTOR *create_actor(uint32_t type, TRANSFORM initial_transform);

void render_actor(ACTOR *actor);

void destroy_actor(ACTOR *actor);

void submit_scene();

extern uint32_t ambient_light;

#endif