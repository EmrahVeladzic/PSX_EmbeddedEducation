#ifndef MODEL_H
#define MODEL_H

#include <gte.h>

#define MDL_NO_DATA 255

typedef enum ast_flag : uint8_t{

    MDL_FLAG_ANIMATIONS = 1 << 1,
    MDL_FLAG_SKELETON   = 1 << 2,
    MDL_FLAG_NORMALS    = 1 << 3,
    MDL_FLAG_UVS        = 1 << 4,
    MDL_FLAG_INDICES    = 1 << 5,
    MDL_FLAG_VERTICES   = 1 << 6,
    MDL_FLAG_MODEL      = 1 << 7 

}AST_FLAG;

typedef struct anim_track{

    VEC3 *translations;
    QUAT *rotations;
    SVEC3 *scales;
    uint8_t *frames;

    uint8_t t_count;
    uint8_t r_count;
    uint8_t s_count;

}ANIM_TRACK;


typedef struct joint{

    TRANSFORM init_trans;
    AFFINE active_affine;

    ANIM_TRACK *tracks;

    uint8_t *child_indices;

    uint8_t child_count;  
    

}JOINT;

typedef struct rig{

    JOINT *joints;

    uint8_t fps;
    uint8_t anim_count;
    uint8_t joint_count;
    uint8_t root_index;

}RIG;



typedef struct model{

    MSH_ARGS *meshes;
    RIG *skeletal_rig;
    uint8_t flag;
    uint8_t mesh_count;

}MODEL;


typedef struct ast_mem {
    MODEL* model;
    struct ast_mem* next;
    struct ast_mem* prev;
    char *path;
}AST_Allocation;

typedef struct anim_ctrl
{   
    uint8_t t_current;
    uint8_t r_current;
    uint8_t s_current;
}ANIM_CONTROLLER;


AST_Allocation *load_asset(const char *path);

void clear_asset(AST_Allocation *asset);

#endif