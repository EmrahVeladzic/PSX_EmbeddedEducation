#include <scene.h>


#define OT_SLOTS    2048          
#define POOL_WORDS  16384    

static uint32_t ot[2][OT_SLOTS];        
static uint32_t pool[2][POOL_WORDS];    


static TRANSFORM camera = {
    .translation = { 0, 0, 4000 },
    .rotation    = { 4096, 0, 0, 0 },
    .scale       = { 4096, 4096, 4096 }
};
static int16_t orbit_angle = 0;

uint32_t ambient_light = 0x00707070;


static DRAW_CTX draw_context;

ACTOR *scene_actors[MAX_ACTORS];

volatile bool release = true;

static volatile int frame = 0;

volatile bool second_framebuffer = false;


void vblank_irq(void){

    release = true;
    frame++;
    if(frame==TARGET_FPS){
        frame = 0;
    }
}

int get_frame(void){
    return frame;
}



void setup(void){

   SPU_Allocation *sfx = load_audio("ASSETS/RAVEN.WL",0);
    
    if(sfx){
        spu_play(0,sfx->origin,sfx->sample_rate);
    }

    load_texture("ASSETS/FONT.RPF");

    s_clear_color = 0x505050;

    TRANSFORM skeleton = {
        .translation = { -950, -6, -150 },
        .rotation    = { 0, 1060, 0, 3956 },      
        .scale       = { 4096, 4096, 4096 }
    };

    TRANSFORM raven = {
        .translation = { 850, -393, -750 },
        .rotation    = { 0, -1891, 0, 3633 },      
        .scale       = { 4096, 4096, 4096 }
    };

    TRANSFORM character = {
    .translation = { 0, -74, 450 },
    .rotation    = { 0, -2494, 0, 3250},         
    .scale       = { 4096, 4096, 4096 }
    };

    TRANSFORM minion = {
    .translation = { 950, -6, -150 },
    .rotation    = { 0, 3250, 0, 2494 },
    .scale       = { 4096, 4096, 4096 }
    };

    create_actor (A_TYP_SKELETON, skeleton);

    create_actor(A_TYP_RAVEN,raven);

    create_actor(A_TYP_PLAYER,character);

    create_actor(A_TYP_MINION,minion);

}

ACTOR *create_actor(uint32_t type, TRANSFORM initial_transform){

    for (size_t i = 0; i < MAX_ACTORS; i++)
    {

        if(!scene_actors[i]){

            ACTOR * actor = create_specific_actor(type);

            if(actor){

                actor->actor_transform=initial_transform;
                scene_actors[i] = actor;
                return actor;

            }

        }


    }
    

    return NULL;

}

void destroy_actor(ACTOR *actor){

    bool destroy_asset = true;
    bool destroy_texture = true;

    for (size_t i = 0; i < MAX_ACTORS; i++)
    {
        if(scene_actors[i]==actor){
            scene_actors[i]=NULL;

        }
        else if(scene_actors[i]){

            if(scene_actors[i]->asset==actor->asset){
                destroy_asset=false;
            }

            if(scene_actors[i]->texture==actor->texture){
                destroy_texture=false;
            }


        }

    }

    if(destroy_asset){
        clear_asset(actor->asset);
    }
    if(destroy_texture){
        clear_texture(actor->texture);
    }
    
    free(actor);

}

static void set_active_affine(RIG *rig, AFFINE parent_affine, uint8_t index){

    if(rig && rig->joints && rig->joint_count>index){

        rig->joints[index].active_affine = multiply_affine(&parent_affine,&rig->joints[index].active_affine);

        if(rig->joints[index].child_count && rig->joints[index].child_indices){

            for (uint8_t i = 0; i < rig->joints[index].child_count; i++)
            {
                set_active_affine(rig,rig->joints[index].active_affine,rig->joints[index].child_indices[i]);
            }
            

        }


    }


}

static uint16_t channel_factor(const uint8_t *frames, uint8_t count, uint8_t *cursor, uint8_t *next_out, int16_t local, int16_t fps){
 
    uint8_t current = *cursor;
    uint8_t next = current + 1;
 
    if(next >= count){
        next = 0;
    }
 
    *next_out = next;
 
    int16_t gap = (int16_t)frames[next] - (int16_t)frames[current];
 
    if(gap < 0){
        gap += fps;
    }
 
    int16_t elapsed = local - (int16_t)frames[current];
 
    if(elapsed < 0){
        elapsed += fps;
    }
 
    if(elapsed >= gap){
        *cursor = next;
        return 4096;
    }
 
    return (uint16_t)(elapsed * recip[gap]);
}
 


void render_actor(ACTOR *actor){
 
 
    AFFINE c = to_affine(&camera);
    AFFINE a = to_affine(&actor->actor_transform);
 
    AFFINE base = multiply_affine(&c,&a);
 
    load_affine(&base);
 
    if (actor->texture)
    {
        draw_context.color_cmd = (0x24 << 24) | ambient_light;
        draw_context.tpage = actor->texture->texpage << 16;
        draw_context.clut = actor->texture->clut_word << 16;
    }
    else{
        draw_context.color_cmd = (0x20 << 24) | 0xFFFFFF;
    }
 
    RIG *rig = actor->asset->model->skeletal_rig;
 
    if(rig&&rig->joint_count>rig->root_index&&rig->joints){
 
        if(actor->anim_select<rig->anim_count&&actor->animation_controller&&actor->animate){
 
            int16_t fps = rig->fps;
 
            int16_t local = (uint16_t)frame - (int16_t)actor->relative_start_frame;
 
            if(local < 0){
                local += fps;
            }
 
            for (size_t i = 0; i < rig->joint_count; i++)
            {
 
                const ANIM_TRACK *track = &rig->joints[i].tracks[actor->anim_select];
                ANIM_CONTROLLER *ctrl = &actor->animation_controller[i];
 
                TRANSFORM inter = rig->joints[i].init_trans;
 
                uint8_t current;
                uint8_t next;
                uint16_t factor;
 
                if(track->t_count){
 
                    current = ctrl->t_current;
 
                    factor = channel_factor(track->frames, track->t_count, &ctrl->t_current, &next, local, fps);
 
                    if(factor >= 4096){
                        inter.translation = track->translations[next];
                    }
                    else{
                        inter.translation = interpolate_vec3(&track->translations[current], &track->translations[next], factor);
                    }
                }
 
                if(track->r_count){
 
                    current = ctrl->r_current;
 
                    factor = channel_factor(track->frames + track->t_count, track->r_count, &ctrl->r_current, &next, local, fps);
 
                    if(factor >= 4096){
                        inter.rotation = track->rotations[next];
                    }
                    else{
                        inter.rotation = interpolate_quat(&track->rotations[current], &track->rotations[next], factor);
                    }
                }
 
                if(track->s_count){
 
                    current = ctrl->s_current;
 
                    factor = channel_factor(track->frames + track->t_count + track->r_count, track->s_count, &ctrl->s_current, &next, local, fps);
 
                    if(factor >= 4096){
                        inter.scale = track->scales[next];
                    }
                    else{
                        inter.scale = interpolate_svec3(&track->scales[current], &track->scales[next], factor);
                    }
                }
 
                rig->joints[i].active_affine = to_affine(&inter);
 
            }
 
        }
        else{
 
            for (size_t i = 0; i < rig->joint_count; i++)
            {
                rig->joints[i].active_affine = to_affine(&rig->joints[i].init_trans);
            }
 
        }
 
        set_active_affine(rig,base,rig->root_index);
    }
 
 
    for (size_t i = 0; i < actor->asset->model->mesh_count; i++)
    {
 
        if(rig&&actor->asset->model->meshes[i].bone_index<rig->joint_count){
 
            load_affine(&rig->joints[actor->asset->model->meshes[i].bone_index].active_affine);
        }
        else{
            load_affine(&base);
        }
 
 
        draw_context.packet = queue_mesh(&actor->asset->model->meshes[i],&draw_context);
 
    }
 
 
}

void submit_scene(void){
    execute_scene_draw(ot[second_framebuffer], OT_SLOTS, second_framebuffer ? F_BUFF_X0 : F_BUFF_X1, second_framebuffer ? F_BUFF_Y0 : F_BUFF_Y1);
}


void tick(int last_frame){

    orbit_angle = (int16_t)((orbit_angle + 8) & 4095);   
    int16_t half = (int16_t)(orbit_angle >> 1);
    camera.rotation = (QUAT){ fx_cos(half), 0, (int16_t)(-fx_sin(half)), 0 };


    for (size_t i = 0; i < MAX_ACTORS; i++)
    {

        if(scene_actors[i] && scene_actors[i]->tick){

            scene_actors[i]->tick(scene_actors[i],last_frame);

        }


    }

    start_dma_transfer(DMA_CH_OTC, &ot[second_framebuffer? 1 : 0][OT_SLOTS - 1], OT_SLOTS, DMA_STEP_BACKWARD | DMA_START | DMA_FORCE, true);

    draw_context.ot = ot[second_framebuffer? 1: 0];
    draw_context.packet = pool[second_framebuffer? 1: 0];
    draw_context.packet_end = pool[second_framebuffer? 1: 0] + POOL_WORDS;
    
    for (size_t i = 0; i < MAX_ACTORS; i++)
    {

        if(scene_actors[i] && scene_actors[i]->asset && scene_actors[i]->asset->model && scene_actors[i]->asset->model->mesh_count && scene_actors[i]->asset->model->meshes){

            render_actor(scene_actors[i]);

        }


    }

}

