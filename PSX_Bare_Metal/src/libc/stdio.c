#include <stdio.h>
#include <cdrom.h>
#include <dma.h>
#include <hardware.h>

#define SECTOR_COUNT(x) (((x) + DATA_SECTOR_SIZE - 1) / DATA_SECTOR_SIZE)
#define PATH_ENTRY_SIZE(x) (8 + ((x) + ((x) & 0x1)))
#define DIRECTORY_ENTRY_SIZE(x) (33 + ((x) + !((x) & 0x1)))

bool system_ready = false;

char *current_sector = NULL;

uint8_t current_minute=0;
uint8_t current_second=0;
uint8_t current_frame=0;


typedef enum file_mode : uint8_t {
    FM_INVALID_MODE = 0x0,
    FM_CDROM_READ_BINARY = 0x1,

} F_MODE;

typedef struct iso_p_entry{
    uint32_t lba;
    uint16_t parent; 
    char     name[9]; 
} PathEntry;

PathEntry *path_table = NULL;
size_t path_count = 0;

void read_lba(uint32_t lba){
    SECTOR_TIME timestamp = from_lba(lba);
    current_minute=timestamp.minute;
    current_second=timestamp.second;
    current_frame=timestamp.frame;

    
    cdrom_issue_cmd(CD_CMD_SETMODE,1,(CD_ARGUMENT[]){CD_MODE_DOUBLE_SPEED|CD_MODE_SIZE_2048},CD_IRQ_S_ACK);
    cdrom_issue_cmd(CD_CMD_SETLOC,3,(uint8_t[]){current_minute,current_second,current_frame},CD_IRQ_S_ACK);
    cdrom_issue_cmd(CD_CMD_READ_N,0,NULL,CD_IRQ_S_DATA_RDY);

    _MMIO8(CD_REG0) = CD_R0_BANK0;
    _MMIO8(CD_REG3) = CD_BFRD;

    while(!(_MMIO8(CD_REG0) & CD_DATA_REQ)){__asm__ volatile("");} 
    start_dma_transfer(DMA_CH_CDROM,current_sector,DATA_SECTOR_SIZE>>2,DMA_START|DMA_FORCE);   
    cdrom_issue_cmd(CD_CMD_PAUSE,0,NULL,CD_IRQ_S_ACK);


}

void init_system(){
    if(!cdrom_up){
        cdrom_init();
    }

    current_sector=malloc(DATA_SECTOR_SIZE);
    if(!current_sector){return;}

    read_lba(16);


    int32_t p_table_size;
    memcpy(&p_table_size, current_sector + 132, 4);

    uint8_t *p_buffer = malloc(SECTOR_COUNT(p_table_size)*DATA_SECTOR_SIZE);

    if(!p_buffer){
        return;
    }

    uint32_t p_table_sector_end = SECTOR_COUNT(p_table_size);

    for (uint32_t i = 0; i < p_table_sector_end; i++)
    {
        read_lba(i+18);
        memcpy(p_buffer+(i*DATA_SECTOR_SIZE),current_sector,DATA_SECTOR_SIZE);
       
    }

    uint32_t inc = 0;

    for (uint32_t i = 0; i < p_table_size; i+=inc)
    {
        inc = PATH_ENTRY_SIZE(p_buffer[i]);

        path_count ++;
        path_table = realloc(path_table,path_count*sizeof(PathEntry));

        if(!path_table){return;}

        memcpy(&path_table[path_count-1].lba, (p_buffer+ i + 2), 4);

        memcpy(&path_table[path_count-1].parent, (p_buffer+ i + 6), 2);

        path_table[path_count-1].parent--;

        memcpy(&path_table[path_count-1].name, (p_buffer+ i + 8), p_buffer[i]);

        path_table[path_count-1].name[p_buffer[i]]='\0';


    }

  
    
    free(p_buffer);

    system_ready = true;
}

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
  
    char prefix[9];
    prefix[8]='\0';
    size_t len = strlen(path);
    char *suffix = malloc(len+1);
    char *init_suffix = suffix;
    char file_name [13];
    memcpy(suffix,path,len);
    suffix[len]='\0';
   
    size_t sub_len = 0;

    uint16_t dir = 0;

    do
    {
        len = strlen(suffix);
       
        char *temp;

        sub_len = split_next(suffix,'/',&temp);

        size_t copy_len = (len > sub_len) ? (len - sub_len - 1) : 0;
        if (copy_len > 8) { copy_len = 8; }

        memcpy(prefix, suffix, copy_len);
        prefix[copy_len] = '\0';

        for (size_t i = 0; i < path_count; i++)
        {
            if(path_table[i].parent==dir && streq(path_table[i].name,prefix)){
                dir = i;
            }
        }
        

        suffix = temp;

    } while (len!=sub_len);  

    memcpy(file_name,suffix,strlen(suffix));
    file_name[strlen(suffix)]='\0';


    free(init_suffix);

    read_lba(path_table[dir].lba);

    uint32_t inc = 0;

    uint32_t lba = 0;
    uint32_t data_len = 0;
    
    char dir_entry [13];
    dir_entry[12]='\0';
    for (uint32_t i = 0; i < DATA_SECTOR_SIZE && current_sector[i]; i+=inc)
    {
        inc = DIRECTORY_ENTRY_SIZE(current_sector[i]);

        uint8_t nl = current_sector[i+32]-1;

        if (nl > 12) {nl = 12;}

        memcpy(dir_entry,(current_sector+i+33),nl);

        dir_entry[nl-1]='\0';
       


        if(streq(file_name,dir_entry)){

            memcpy(&lba,current_sector+i+2,4);
            memcpy(&data_len,current_sector+i+10,4);
        }
    }



    if(lba&&data_len){

        FILE * output = malloc(sizeof(FILE));

        if(!output){return NULL;}

        output->lba=lba;
        output->len_bytes=data_len;
        output->current_offset=0;

        read_lba(lba);

        return output;
    }

    return NULL;
}

FILE *fopen(const char *path, const char *mode){
    if(!system_ready){
        init_system();
    }
    if(!(path&&mode)){return NULL;}

    F_MODE md = FM_INVALID_MODE;
    if(streq(mode,"rb")){
        md=FM_CDROM_READ_BINARY;
    }
    if(md==FM_INVALID_MODE){return NULL;}
    return fopen_internal(path,md);
}


size_t fread(void *dest, size_t size, size_t amount, FILE *fptr);

int fclose(FILE *fptr){
    fptr->current_offset=0;
    return 0;
}
