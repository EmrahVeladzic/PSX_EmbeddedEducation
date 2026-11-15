#ifndef GTE_H
#define GTE_H

#include <stdint.h>

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

typedef struct gte_msh_args{

    VTX8 *vertices;
    NRM8 *normals;
    uint16_t *indices;
    uint16_t vertex_count;
    uint16_t triangle_count;

}MSH_ARGS;




#endif