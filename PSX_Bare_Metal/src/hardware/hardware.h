#ifndef HARDWARE_H
#define HARDWARE_H

#include <dma.h>
#include <cdrom.h>
#include <interrupts.h>

#define CPU_FREQ 0x204CC00

extern void delay_microseconds(size_t microseconds);

char *ram_debug(const char *msg);

#endif