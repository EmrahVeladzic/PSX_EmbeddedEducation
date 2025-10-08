#ifndef MMIO_H
#define MMIO_H

    #include <stdint.h>

    #define _ADDR8(input)((volatile uint8_t *)input)
    #define _ADDR16(input)((volatile uint16_t *)input)
    #define _ADDR32(input)((volatile uint32_t *)input)

    #define _MMIO8(input)(*_ADDR8(input))
    #define _MMIO16(input)(*_ADDR16(input))
    #define _MMIO32(input)(*_ADDR32(input))

    #define MASK_TOGGLE8(addr,mask,state)do{if(state){_MMIO8(addr)|=mask;}else{_MMIO8(addr)&=~mask;}}while(0)
    #define MASK_TOGGLE16(addr,mask,state)do{if(state){_MMIO16(addr)|=mask;}else{_MMIO16(addr)&=~mask;}}while(0)
    #define MASK_TOGGLE32(addr,mask,state)do{if(state){_MMIO32(addr)|=mask;}else{_MMIO32(addr)&=~mask;}}while(0)
    
    

#endif
