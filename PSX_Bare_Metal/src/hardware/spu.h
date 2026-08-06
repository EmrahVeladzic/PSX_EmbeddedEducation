#ifndef SPU_H
#define SPU_H

#include <mmio.h>

#define SPU_VOICE_VOLUME_LEFT(voice,value){_MMIO16(0x1F801C00+(voice*0x10))=value;}
#define SPU_VOICE_VOLUME_RIGHT(voice,value){_MMIO16(0x1F801C02+(voice*0x10))=value;}

#define SPU_VOICE_VOLUME_BOTH(voice,value)do{SPU_VOICE_VOLUME_LEFT(voice,value);SPU_VOICE_VOLUME_RIGHT(voice,value);}while(false)

#define SPU_PITCH_STOP 0x0
#define SPU_PITCH_STANDARD 0x1000
#define SPU_PITCH_FAST 0x4000

#define SPU_VOICE_PITCH(voice,value){_MMIO16(0x1F801C04+(voice*0x10))=value;}

#define SPU_MASTER_VOLUME_LEFT(value){_MMIO16(0x1F801D80)=value;}
#define SPU_MASTER_VOLUME_RIGHT(value){_MMIO16(0x1F801D82)=value;}

#define SPU_MASTER_VOLUME_BOTH(value)do{SPU_MASTER_VOLUME_LEFT(value);SPU_MASTER_VOLUME_RIGHT(value);}while(false)

#define SPU_KEY_ON(voice)do{if(voice>0xF){_MMIO16(0x1F801D8A)=(0x1<<((voice-0x10)&0xF));}else{_MMIO16(0x1F801D88)=(0x1<<((voice)&0xF));}}while(false)
#define SPU_KEY_OFF(voice)do{if(voice>0xF){_MMIO16(0x1F801D8E)=(0x1<<((voice-0x10)&0xF));}else{_MMIO16(0x1F801D8C)=(0x1<<((voice)&0xF));}}while(false)


#define SPU_VOICE_START(voice,addr){_MMIO16(0x1F801C06+(voice*0x10))=addr;}

#define SPU_PMON(voice,state)do{if(voice>0xF){MASK_TOGGLE16(0x1F801D92,(0x1<<(voice-0x10)),state);}else{MASK_TOGGLE16(0x1F801D90,(0x1<<voice),state);}}while(false)
#define SPU_NON(voice,state)do{if(voice>0xF){MASK_TOGGLE16(0x1F801D96,(0x1<<(voice-0x10)),state);}else{MASK_TOGGLE16(0x1F801D94,(0x1<<voice),state);}}while(false)
#define SPU_EON(voice,state)do{if(voice>0xF){MASK_TOGGLE16(0x1F801D9A,(0x1<<(voice-0x10)),state);}else{MASK_TOGGLE16(0x1F801D98,(0x1<<voice),state);}}while(false)

#define SPU_VOICE_ADSR(voice,lo,hi)do{_MMIO16(0x1F801C08+(voice*0x10))=lo;_MMIO16(0x1F801C0A+(voice*0x10))=hi;}while(false)

#define SPU_XFER_ADDR(addr){_MMIO16(0x1F801DA6)=addr;}
#define SPU_XFER_CTRL(value){_MMIO16(0x1F801DAC)=value;}

#define SPU_CNT_WRITE(value){_MMIO16(0x1F801DAA)=value;}

#define SPU_CNT_READ() (_MMIO16(0x1F801DAA))
#define SPU_STAT_READ() (_MMIO16(0x1F801DAE))

#define SPU_PITCH_FROM_RATE(rate) ((uint16_t)((((uint32_t)(rate) * 4096) / 44100) > 0x4000 ? 0x4000 : (((uint32_t)(rate) * 4096) / 44100)))

#define SPU_DMA_BCR(blocks) ((((blocks) / 4) << 16) | 16)

typedef enum spucnt_value : uint16_t{
    SPU_OFF = 0x0000,
    SPU_ENABLE = 0x8000,
    SPU_DEMUTE = 0x4000,
    SPU_REVERB = 0x0080,    
    SPU_IRQ = 0x0040, 
    SPU_DMA_R = 0x0030,
    SPU_DMA_W = 0x0020,
    SPU_MANUAL_W = 0x0010,
    SPU_EXT_REVERB = 0x0008,
    SPU_CD_REVERB = 0x0004,
    SPU_EXT_ENABLE = 0x0002,
    SPU_CD_ENABLE = 0x0001
}SPUCNT_V;

typedef enum spustat_value : uint16_t {
    SPUSTAT_CAPTURE_HALF   = 0x0800,  
    SPUSTAT_XFER_BUSY      = 0x0400,  
    SPUSTAT_DMA_READ_REQ   = 0x0200,
    SPUSTAT_DMA_WRITE_REQ  = 0x0100,
    SPUSTAT_DMA_RW_REQ     = 0x0080,
    SPUSTAT_IRQ9           = 0x0040,
    SPUSTAT_MODE_MASK      = 0x003F   
} SPUSTAT_V;

typedef enum spu_xfer_type : uint16_t {
SPU_XFER_FILL   = 0x0000,
SPU_XFER_NORMAL = 0x0004,
SPU_XFER_REP2   = 0x0006,
SPU_XFER_REP4   = 0x0008,
SPU_XFER_REP8   = 0x000A
} SPU_XFER_T;

extern bool spu_up;

void spu_irq(void);
void spu_init(void);

bool load_audio(void *buffer, uint32_t blocks, uint16_t sample_rate, uint8_t voice);

#endif