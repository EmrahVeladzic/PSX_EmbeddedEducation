#ifndef GTE_H
#define GTE_H

#include <stdint.h>

#define PROJ_DIST 275
#define Z_SCALE_FACTOR 341

typedef struct vec3{

    int32_t x;
    int32_t y;
    int32_t z;

}VEC3;

typedef struct svec3{
    
    int16_t x;
    int16_t y;
    int16_t z;

}SVEC3;

typedef struct quat{
    
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t w;

}QUAT;



typedef struct mat3x3{

    alignas(4) int16_t r11;
    int16_t r12;
    int16_t r13;

    int16_t r21;
    int16_t r22;
    int16_t r23;

    int16_t r31;
    int16_t r32;
    int16_t r33;

}MAT3X3;


typedef struct vtx8{

    SVEC3 vec;
    uint8_t u;
    uint8_t v;

}VTX8;

typedef struct nrm8{

    SVEC3 vec;
    int16_t pad;

}NRM8;

typedef struct transform{

    VEC3 translation;
    QUAT rotation;
    SVEC3 scale;

}TRANSFORM;

typedef struct affine{

    VEC3 translation;
    MAT3X3 rotation_scale;

}AFFINE;

typedef struct gte_msh_args{

    VTX8 *vertices;
    NRM8 *normals;
    uint16_t *indices;
    uint16_t vertex_count;
    uint16_t triangle_count;

}MSH_ARGS;

typedef struct draw_ctx{
    uint32_t *ot;           
    uint32_t *packet;       
    uint32_t *packet_end;   
    uint32_t color_cmd;     
    uint32_t clut;          
    uint32_t tpage;         
}DRAW_CTX;

void load_affine(const AFFINE *aff);

void load_transform(const TRANSFORM *trans);

void load_matrix(const MAT3X3 *matrix);

void load_vector(const VEC3 *vector);

MAT3X3 get_matrix(void);

VEC3 get_vector(void);

AFFINE get_affine(void);

AFFINE multiply_with_current(const AFFINE *input);

AFFINE create_set_affine(const TRANSFORM *transform);

void set_screen_offset(int32_t ofx, int32_t ofy, uint16_t h, uint16_t zsf3);

uint32_t *queue_mesh(const MSH_ARGS *mesh, const DRAW_CTX *ctx);

#endif