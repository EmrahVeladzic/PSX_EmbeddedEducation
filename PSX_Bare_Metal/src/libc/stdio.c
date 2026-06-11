#include <stdio.h>
#include <cdrom.h>
#include <dma.h>


FILE *open_file = NULL;
uint8_t current_minute = 0;
uint8_t current_second = 0;
uint8_t current_frame = 0;
size_t current_byte = 0;

typedef enum file_mode : uint8_t {
    FM_INVALID_MODE = 0x0,
    FM_CDROM_READ_BINARY = 0x1,

} F_MODE;



uint8_t to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

SECTOR_TIME from_lba(size_t lba){
    size_t total = lba +150;
    SECTOR_TIME output;
    output.frame = to_bcd((uint8_t)(total % 75));
    total/=75;
    output.second = to_bcd((uint8_t)(total % 60));
    total/=60;
    output.minute = to_bcd((uint8_t)(total));
    return output;
}


FILE *fopen_internal(const char *path, F_MODE mode){
    open_file=malloc(DATA_SECTOR_SIZE);
    if(!open_file){return NULL;}

    cdrom_issue_cmd(CD_CMD_SETMODE,1,(CD_ARGUMENT[]){CD_MODE_DOUBLE_SPEED|CD_MODE_SIZE_2048},CD_IRQ_S_ACK);
    cdrom_issue_cmd(CD_CMD_SETLOC,3,(uint8_t[]){to_bcd(0),to_bcd(2),to_bcd(16)},CD_IRQ_S_ACK);
    cdrom_issue_cmd(CD_CMD_READ_N,0,NULL,CD_IRQ_S_DATA_RDY);

    _MMIO8(CD_REG0) = CD_R0_BANK0;
    _MMIO8(CD_REG3) = CD_BFRD;

    while(!(_MMIO8(CD_REG0) & CD_DATA_REQ)){__asm__ volatile("");} 

    start_dma_transfer(DMA_CH_CDROM,open_file,DATA_SECTOR_SIZE>>2,DMA_START|DMA_FORCE);   
    cdrom_issue_cmd(CD_CMD_PAUSE,0,NULL,CD_IRQ_S_ACK);



    return open_file;   
}

FILE *fopen(const char *path, const char *mode){
    if(!cdrom_up){
        cdrom_init();
    }
    F_MODE md = FM_INVALID_MODE;
    if(streq(mode,"rb")){
        md=FM_CDROM_READ_BINARY;
    }
    if(md==FM_INVALID_MODE){return NULL;}
    return fopen_internal(path,md);
}


size_t fread(void *dest, size_t size, size_t amount, FILE *fptr);

int fclose(FILE *fptr){
    open_file=NULL;
    current_minute=0;
    current_second=0;
    current_second=0;
    return 0;
}
