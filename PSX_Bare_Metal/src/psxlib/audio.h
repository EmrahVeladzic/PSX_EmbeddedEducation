#ifndef AUDIO_H
#define AUDIO_H

#include <spu.h>

typedef struct spu_mem {
    uint32_t block_count;
    uint32_t origin;
    struct spu_mem *next;
    struct spu_mem *prev;   
    char *path;
    uint16_t sample_rate;
    uint8_t channel;
    bool free;
} SPU_Allocation;

SPU_Allocation *load_audio(const char *path, uint8_t channel);

void clear_audio(SPU_Allocation *audio);


#endif