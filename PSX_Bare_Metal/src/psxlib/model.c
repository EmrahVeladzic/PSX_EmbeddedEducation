#include <model.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unlocked.h>


static AST_Allocation* AssetBase = NULL;

static void ** allocation_tracker = NULL;
static size_t allocation_count = 0;

static void* attempt_allocation(size_t size){

    if(!size){return NULL;}

    void ** change = realloc(allocation_tracker,(allocation_count+1)*sizeof(void*));

    if(change){

        allocation_tracker = change;

        allocation_count++;

        void * new_allc = malloc(size);

        if(new_allc){

            allocation_tracker[allocation_count-1]=new_allc;

            return new_allc;
        }
      

        allocation_count--;

        if (allocation_count == 0) {
            free(allocation_tracker);
            allocation_tracker = NULL;
        }
        else {
            change = realloc(allocation_tracker, allocation_count * sizeof(void*));

            if (change) {
                allocation_tracker = change;
            }
        }
        
    }

    return NULL;

} 

static void clear_allocations(){

    for (size_t i = 0; i < allocation_count; i++)
    {
        free(allocation_tracker[i]);
    }
    free(allocation_tracker);
    allocation_tracker=NULL;
    allocation_count=0;   

}


AST_Allocation *load_asset(const char *path){


    AST_Allocation *allc = AssetBase;

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

    AST_Allocation *new_allc = malloc(sizeof(AST_Allocation));
    

    if(new_allc){

        new_allc ->next = NULL;
        new_allc ->prev = NULL;
        new_allc ->model = NULL;
        new_allc ->path = p_copy;

    }

    else{

        free(p_copy);
        return NULL;

    }

    FILE *file = NULL; 

    bool fail = true;

    file = fopen(path, "rb");
    
    uint8_t temp_byte = 0;

    uint16_t temp_ushort = 0;

    void *temp_allc;

    if(file){

        fread(&temp_byte,1,1,file);

        if(temp_byte=='A'){

            temp_allc = attempt_allocation(sizeof(MODEL));

            if(temp_allc){

                new_allc->model=temp_allc;
                new_allc->model->meshes=NULL;
                new_allc->model->skeletal_rig=NULL;
                new_allc->model->mesh_count=0;
                new_allc->model->flag=0;

                fread(&temp_byte,1,1,file);

                new_allc->model->flag=temp_byte;

                if(temp_byte & MDL_FLAG_MODEL && temp_byte & MDL_FLAG_VERTICES && temp_byte & MDL_FLAG_INDICES){

                    fseek(file,5,SEEK_CUR);

                    fread(&temp_byte,1,1,file);

                    temp_allc = attempt_allocation(temp_byte * sizeof(MESH));

                    if(temp_allc){
                        new_allc->model->mesh_count=temp_byte;
                        new_allc->model->meshes = temp_allc;

                        fail = false;

                        for (size_t i = 0; i < new_allc->model->mesh_count; i++)
                        {
                            if(fail){break;}
                            fail = true;

                           
                            fread(&temp_byte,1,1,file);

                            new_allc->model->meshes[i].bone_index=temp_byte;
                            
                            fseek(file,1,SEEK_CUR);

                            fread(&temp_ushort,2,1,file);

                            new_allc->model->meshes[i].data.vertex_count = temp_ushort;

                            temp_allc = attempt_allocation(temp_ushort * sizeof(VTX8));                           
                                
                            if(fread(temp_allc,sizeof(VTX8),temp_ushort,file)==temp_ushort){

                                new_allc->model->meshes[i].data.vertices=temp_allc;

                                fread(&temp_ushort,2,1,file);

                                fseek(file,2,SEEK_CUR);

                                temp_allc = attempt_allocation(temp_ushort * sizeof(NRM8));

                                if(fread(temp_allc,sizeof(NRM8),temp_ushort,file)==temp_ushort){

                                    new_allc->model->meshes[i].data.normals=temp_allc;

                                    fread(&temp_ushort,2,1,file);

                                    fseek(file,2,SEEK_CUR);

                                    new_allc->model->meshes[i].data.triangle_count=temp_ushort/3;

                                    temp_allc = attempt_allocation(temp_ushort * 2);

                                    if(fread(temp_allc,2,temp_ushort,file)==temp_ushort){

                                        new_allc->model->meshes[i].data.indices=temp_allc;


                                        fseek(file,(temp_ushort&0x1)*2,SEEK_CUR);


                                        fail = false;

                                    }


                                }


                            }
                              
                            

                        }             

                        if(!fail && new_allc->model->flag & MDL_FLAG_SKELETON){
                           
                            fail = true;


                            fread(&temp_byte,1,1,file);

                            temp_allc = attempt_allocation(sizeof(RIG));

                            if(temp_allc){

                                new_allc->model->skeletal_rig=temp_allc;

                                new_allc->model->skeletal_rig->joint_count=temp_byte;

                                temp_allc = attempt_allocation(temp_byte*sizeof(JOINT));

                                if(temp_allc){

                                    new_allc->model->skeletal_rig->joints=temp_allc;

                                    fread(&temp_byte,1,1,file);

                                    new_allc->model->skeletal_rig->root_index=temp_byte;

                                    fread(&temp_byte,1,1,file);

                                    new_allc->model->skeletal_rig->anim_count = temp_byte;

                                    fread(&temp_byte,1,1,file);

                                    new_allc->model->skeletal_rig->fps = temp_byte;
                                    
                                    fail = false;

                                    for (uint8_t i = 0; i < new_allc->model->skeletal_rig->joint_count; i++)
                                    {
                                        if(fail){break;}

                                        fail = true;                                
                                        
                                        if(fread(&new_allc->model->skeletal_rig->joints[i].init_trans,26,1,file)==1){
                                        
                                            fread(&temp_byte,1,1,file);

                                            new_allc->model->skeletal_rig->joints[i].child_count=temp_byte;

                                            temp_allc = attempt_allocation(temp_byte);

                                            new_allc->model->skeletal_rig->joints[i].child_indices = temp_allc;

                                            if(fread(temp_allc,1,temp_byte,file)==temp_byte){

                                                temp_byte+=3;
                                                temp_byte &= 0x3;

                                                fseek(file,temp_byte? (4-temp_byte): 0,SEEK_CUR);

                                                temp_allc = attempt_allocation(new_allc->model->skeletal_rig->anim_count * sizeof(ANIM_TRACK));

                                                new_allc->model->skeletal_rig->joints[i].tracks = temp_allc;

                                                if(!temp_allc && !new_allc->model->skeletal_rig->anim_count){
                                                    fail = false;
                                                }

                                                if(temp_allc){

                                                

                                                    fail = false;

                                                    for (uint8_t j = 0; j < new_allc->model->skeletal_rig->anim_count; j++)
                                                    {

                                                        if(fail){break;}
                                                        
                                                        fail = true;

                                                        fseek(file,1,SEEK_CUR);

                                                        fread(&temp_byte,1,1,file);
                                                        
                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].t_count = temp_byte;

                                                        fread(&temp_byte,1,1,file);
                                                        
                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].r_count = temp_byte;

                                                        fread(&temp_byte,1,1,file);
                                                        
                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].s_count = temp_byte;

                                                        temp_byte = new_allc->model->skeletal_rig->joints[i].tracks[j].t_count;

                                                        temp_allc = attempt_allocation(temp_byte * sizeof(VEC3));

                                                        if(fread(temp_allc,sizeof(VEC3),temp_byte,file)!=temp_byte){
                                                            break;
                                                        }

                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].translations = temp_allc;

                                                        temp_ushort = temp_byte;

                                                        temp_byte = new_allc->model->skeletal_rig->joints[i].tracks[j].r_count;

                                                        temp_allc = attempt_allocation(temp_byte * sizeof(QUAT));

                                                        if(fread(temp_allc,sizeof(QUAT),temp_byte,file)!=temp_byte){
                                                            break;
                                                        }

                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].rotations = temp_allc;

                                                        temp_ushort += temp_byte;

                                                        temp_byte = new_allc->model->skeletal_rig->joints[i].tracks[j].s_count;
                                                        
                                                        temp_allc = attempt_allocation(temp_byte * sizeof(SVEC3));

                                                        if(fread(temp_allc,sizeof(SVEC3),temp_byte,file)!=temp_byte){
                                                            break;
                                                        }

                                                        new_allc->model->skeletal_rig->joints[i].tracks[j].scales = temp_allc;


                                                        temp_ushort += temp_byte;
                                                        
                                                        temp_allc = attempt_allocation(temp_ushort);

                                                        if(fread(temp_allc,1,temp_ushort,file)==temp_ushort){


                                                            new_allc->model->skeletal_rig->joints[i].tracks[j].frames = temp_allc;

                                                            temp_ushort += (temp_byte*2);
                                                            

                                                            temp_ushort &= 0x3;

                                                            fseek(file,temp_ushort? (4-temp_ushort):0, SEEK_CUR);

                                                            fail = false;

                                                        }



                                                        
                                                    }
                                                
                                                }
                                                

                                            }

                                        }

                                    }
                                    
                                }
                            }

                        }

                        
                       


                    }

                }

            }
        
        }

        fclose(file);

    }



    if(fail){
        clear_allocations();
        free(p_copy);
        free(new_allc);
        new_allc = NULL;
    }

    else{
        free(allocation_tracker);
        allocation_count = 0;
        allocation_tracker = NULL;
    }

    if(new_allc){
        
        if(!AssetBase){
            AssetBase = new_allc;
        }

        else{
            allc = AssetBase;

            while (allc->next)
            {
                allc=allc->next;
            }
            
            allc->next = new_allc;
            new_allc->prev = allc;

        }

    }

    return new_allc;
}


void clear_asset(AST_Allocation *asset){

    if(!asset){return;}


    if(asset->prev){
        asset->prev->next=asset->next;
    }
    else{
        AssetBase=asset->next;
    }

    if(asset->next){
        asset->next->prev=asset->prev;
    }

    free(asset->path);

    if (asset->model)
    {
        if(asset->model->meshes){
            for (uint8_t i = 0; i < asset->model->mesh_count; i++)
            {
                if(asset->model->meshes[i].data.vertices){
                    free(asset->model->meshes[i].data.vertices);
                }

                if(asset->model->meshes[i].data.normals){
                    free(asset->model->meshes[i].data.normals);
                }

                if(asset->model->meshes[i].data.indices){
                    free(asset->model->meshes[i].data.indices);
                }
            }
            

            free(asset->model->meshes);
        }

        if(asset->model->skeletal_rig){

            if(asset->model->skeletal_rig->joints){

                for (uint8_t i = 0; i < asset->model->skeletal_rig->joint_count; i++)
                {
                    if(asset->model->skeletal_rig->joints[i].child_indices){
                        free(asset->model->skeletal_rig->joints[i].child_indices);
                    }


                    if(asset->model->skeletal_rig->joints[i].tracks){

                        for (uint8_t j = 0; j < asset->model->skeletal_rig->anim_count; j++)
                        {
                            
                            if(asset->model->skeletal_rig->joints[i].tracks[j].translations){
                                free(asset->model->skeletal_rig->joints[i].tracks[j].translations);

                            }

                            if(asset->model->skeletal_rig->joints[i].tracks[j].rotations){
                                free(asset->model->skeletal_rig->joints[i].tracks[j].rotations);

                            }

                            if(asset->model->skeletal_rig->joints[i].tracks[j].scales){
                                free(asset->model->skeletal_rig->joints[i].tracks[j].scales);

                            }

                            if(asset->model->skeletal_rig->joints[i].tracks[j].frames){
                                free(asset->model->skeletal_rig->joints[i].tracks[j].frames);

                            }

                        }                       


                        free(asset->model->skeletal_rig->joints[i].tracks);
                    }
                    
                }
                

                free(asset->model->skeletal_rig->joints);
            }


            free(asset->model->skeletal_rig);
        }



        free(asset->model);
    }
    

    free(asset);

    return;

}


