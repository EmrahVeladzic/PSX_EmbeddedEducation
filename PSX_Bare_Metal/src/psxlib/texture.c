#include <texture.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unlocked.h>
#include <fxmath.h>

#define VRAM_RECT(d, s)  VRAM_Y(d) + VRAM_H(s) - 1, VRAM_X(d), VRAM_Y(d), VRAM_X(d) + VRAM_W(s) - 1


static GPU_Allocation *TextureBase = NULL;



GPU_Allocation *gpu_malloc(int32_t dest_tex, int32_t size_tex, int32_t dest_clut, int32_t size_clut){

    GPU_Allocation *new_allc = (GPU_Allocation*) malloc(sizeof(GPU_Allocation));

    if(!new_allc){
        return NULL;
    }

    if(!TextureBase){
        TextureBase = new_allc;
        new_allc->dest_tex=dest_tex;
        new_allc->size_tex=size_tex;
        new_allc->dest_clut=dest_clut;
        new_allc->size_clut=size_clut;
        new_allc->texpage=0;
        new_allc->clut_word=0;
        new_allc->path=NULL;
        new_allc->next=NULL;
        new_allc->prev=NULL;
        return new_allc;
    }

    GPU_Allocation *allc = TextureBase;    
    GPU_Allocation *prev = allc->prev;

    COLLISION_AXIS_RESULT res = COLLISION_NONE;

    while (allc)
    {
        res = rect_test(VRAM_RECT(dest_tex, size_tex), VRAM_RECT(allc->dest_tex, allc->size_tex),true);    
        res |= rect_test(VRAM_RECT(dest_tex,size_tex),VRAM_RECT(allc->dest_clut,allc->size_clut),true);
        res |= rect_test(VRAM_RECT(dest_clut, size_clut), VRAM_RECT(allc->dest_tex, allc->size_tex),true);  
        res |= rect_test(VRAM_RECT(dest_clut,size_clut),VRAM_RECT(allc->dest_clut,allc->size_clut),true);
    
        if(res){
            free(new_allc);             
            return NULL;

        }

        prev=allc;
        allc=allc->next;
    }

    new_allc->dest_tex=dest_tex;
    new_allc->size_tex=size_tex;
    new_allc->dest_clut=dest_clut;
    new_allc->size_clut=size_clut;
    new_allc->prev=prev;
    new_allc->next=NULL;
    new_allc->path=NULL;
    new_allc->texpage=0;
    new_allc->clut_word=0;
    prev->next=new_allc;

    return new_allc;

    
}


GPU_Allocation *load_texture(const char *path){

    if(!gpu_init()){
        return NULL;
    }



    GPU_Allocation *allc = TextureBase;

    while (allc)
    {
        if(allc->path && streq(path,allc->path)){
           
            return allc;
            
        }

        allc=allc->next;
    }
    
    size_t p_len = strlen(path)+1;

    char *p_copy = malloc(p_len);

    if(p_copy){
        memcpy(p_copy,path,p_len);
    }
    else{
        return NULL;
    }

    GPU_Allocation *new_allc = NULL;

    FILE *file = NULL; 

    uint8_t *tex = NULL;

    uint16_t *clut = NULL;

    file = fopen(path, "rb");

    if(file){

        uint8_t magic;

        uint8_t width;

        uint8_t height;

        uint8_t clut_c;

        int32_t dest_tex;

        int32_t size_tex;

        int32_t dest_clut;

        int32_t size_clut;

        uint16_t texpage;

        uint16_t clut_word;

        fread(&magic,sizeof(magic),1,file);

        if(magic=='R'){

           
            fread(&width,sizeof(magic),1,file);

            fread(&height,sizeof(magic),1,file);

            fread(&clut_c,sizeof(magic),1,file);

            fread(&dest_tex,sizeof(dest_tex),1,file);

            fread(&size_tex,sizeof(size_tex),1,file);

            fread(&dest_clut,sizeof(dest_clut),1,file);

            fread(&size_clut,sizeof(size_clut),1,file);
            
            fread(&texpage,sizeof(texpage),1,file);

            fread(&clut_word,sizeof(clut_word),1,file);

            uint32_t clut_halfwords = VRAM_W(size_clut) * VRAM_H(size_clut);

            size_t clut_bytes = (size_t)clut_halfwords * sizeof(uint16_t);

            uint32_t tex_halfwords = VRAM_W(size_tex) * VRAM_H(size_tex);

            size_t tex_bytes = (size_t)tex_halfwords * sizeof(uint16_t);

            clut = (uint16_t*) malloc(clut_bytes);

            tex = (uint8_t*) malloc(tex_bytes);

            if(clut && tex){

                new_allc = gpu_malloc(dest_tex,size_tex,dest_clut,size_clut);

                if(new_allc){

                    new_allc->texpage=texpage;
                    new_allc->clut_word=clut_word;
                    new_allc->path=p_copy;

                    size_t c = fread(clut,sizeof(uint16_t),clut_halfwords,file);

                    if(c == clut_halfwords){

                        size_t t = fread(tex,sizeof(uint8_t),tex_bytes,file);

                        if(t == tex_bytes){

                            bool t_up = gpu_upload(dest_tex,size_tex,tex,tex_halfwords);

                            bool c_up = gpu_upload(dest_clut,size_clut,clut,clut_halfwords);

                            if(!(t_up && c_up)){
                                
                                clear_texture(new_allc);
                                new_allc=NULL;

                            }


                        }
                        else{
                            clear_texture(new_allc);
                            new_allc=NULL;
                        }


                    }
                    else{
                        clear_texture(new_allc);
                        new_allc=NULL;
                    }

                   

                    


                }

                else{                    
                    free(p_copy);
                }

            } 
            else{
                free(p_copy);
            }         


        }
        else{
            free(p_copy);
        }

    }
    else{
        free(p_copy);
    }

    if(file){
        fclose(file);
    }
    if(clut){
        free(clut);
    }  
    if(tex){
        free(tex);
    }
    
    return new_allc;

}


void clear_texture(GPU_Allocation *texture){

    if(!texture){return;}

    if(texture->prev){
        texture->prev->next=texture->next;
    }
    else{
        TextureBase=texture->next;
    }

    if(texture->next){
        texture->next->prev=texture->prev;
    }

    free(texture->path);

    free(texture);

    return;
}

