#include <audio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unlocked.h>

static SPU_Allocation *AudioBase = NULL;

static inline bool init_spu_ram(){

    SPU_Allocation *new_base = (SPU_Allocation*) malloc(sizeof(SPU_Allocation));

    if(!new_base){
        return false;
    }
        
    new_base->channel=0;
    new_base->free=true;
    new_base->prev=NULL;
    new_base->next=NULL;
    new_base->sample_rate = 0;
    new_base->origin = (0x1000 >> 4);            
    new_base->block_count = ((0x80000 >> 4) - (0x1000 >> 4));
    new_base->path = NULL;

    AudioBase=new_base;

    return true;

}

SPU_Allocation *spu_malloc(uint32_t block_count){

    SPU_Allocation *new_allc = (SPU_Allocation*) malloc(sizeof(SPU_Allocation));

    SPU_Allocation *allc = AudioBase;    

    while (allc && new_allc)
    {
        if(allc->free && allc->block_count>=block_count){
            
            new_allc->prev=allc;
            new_allc->next=allc->next;           

            new_allc->free=true;
            new_allc->block_count=allc->block_count-block_count;

            if(new_allc->block_count){

                if(new_allc->next){
                    new_allc->next->prev=new_allc;
                }
                
                new_allc->origin=allc->origin+block_count;
                new_allc->path=NULL;
                new_allc->channel=0;
                new_allc->sample_rate=0;

            }
            else{
                free(new_allc);
                new_allc=NULL;
            }      
           
            allc->free=false;
            allc->block_count=block_count;   
            
            if(new_allc){
                allc->next=new_allc;
            }

           
                        
            return allc;
        }

        allc=allc->next;
    }

    free(new_allc); 
    return NULL;

}


SPU_Allocation *load_audio(const char *path, uint8_t channel){

    if(!spu_init()){
        return NULL;
    }

    if(!AudioBase){
        if(!init_spu_ram()){return NULL;}
    }

    SPU_Allocation *allc = AudioBase;

    SPU_Allocation *path_match = NULL;

    while (allc)
    {
        if(allc->path && streq(path,allc->path)){
            path_match=allc;

            if(allc->channel==channel){
                return allc;
            }
        }

        allc=allc->next;
    }
    
    size_t p_len = strlen(path)+1;

    char *p_copy = malloc(p_len);

    if(p_copy){
        memcpy(p_copy,path,p_len);
    }
    else if (!path_match){
        return NULL;
    }

    SPU_Allocation *new_allc = NULL;

    FILE *file = NULL;

    void *data = NULL;    

    file = fopen(path, "rb");

    if(file){

        uint8_t magic;

        fread(&magic,sizeof(magic),1,file);

        if(magic=='W'){

            uint8_t c_count;

            fread(&c_count,sizeof(c_count),1,file);

            uint16_t s_rate;

            fread(&s_rate,sizeof(s_rate),1,file);

            uint32_t b_count;

            fread(&b_count,sizeof(b_count),1,file);            

            if(c_count>channel && (b_count && !(b_count & 0x3))){

                if(!fseek(file,((uint32_t)channel*(b_count*16)),SEEK_CUR)){

                    data=malloc(b_count*16);

                    if(data){

                        if(fread(data,16,b_count,file)==b_count){

                            new_allc = spu_malloc(b_count);

                            if (new_allc) {

                                new_allc->sample_rate = s_rate;
                                new_allc->channel = channel;

                                if (!copy_audio(data, new_allc->origin, new_allc->block_count)) {
                                    clear_audio(new_allc);      
                                    new_allc = NULL;
                                }
                                else if (path_match) {
                                    new_allc->path = path_match->path;
                                }
                                else {
                                    new_allc->path = p_copy;
                                }
                            }

                        }


                    }

                }

            }


        }

    }

    if (p_copy && (!new_allc || new_allc->path != p_copy)){ 
        free(p_copy); 
    }
    if(file){
        fclose(file);
    }
    if(data){
        free(data);
    }  
    
    return new_allc;

}

static void wipe_path(char *path){

    if(!path){return;}

    SPU_Allocation *allc = AudioBase;

    while (allc)
    {
        if(allc->path==path){

           return;

        }       

        allc = allc->next;
    }

    free(path);

}

void clear_audio(SPU_Allocation *audio){

    if(!audio){return;}

    SPU_Allocation *allc = AudioBase;

    char *path = NULL;


    while (allc)
    {
        if(allc==audio){

            path=allc->path;
            audio->path = NULL;

            allc->free=true;  
            allc->channel=0; 
            allc->sample_rate=0;

            while (allc->next && allc->next->free)
            {
                
                SPU_Allocation *del = allc->next;

                allc->next=allc->next->next;

                allc->block_count+=del->block_count;

                if(allc->next){
                    allc->next->prev=allc;
                }

                free(del);
            }
            
            while (allc->prev&&allc->prev->free)
            {
                SPU_Allocation *del = allc;               

                allc=allc->prev;

                allc->block_count+=del->block_count;

                allc->next=del->next;
                if(allc->next){
                    allc->next->prev=allc;
                }

                free(del);
            }
            

            break;
        }       

        allc = allc->next;
    }

    wipe_path(path);

    return;
}

