#include <spawner.h>


ACTOR *create_specific_actor(uint32_t type){

    ACTOR *out = NULL;

    char * a_path = NULL;
    char * t_path = NULL;

    switch (type)
    {
        case A_TYP_RAVEN :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;out->animate=true;out->anim_select=3;}
            a_path = "ASSETS/RAVEN.AST";
            t_path = "ASSETS/RAVEN.RPF";
            break;
        }

        case A_TYP_PLAYER :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;out->animate=true;out->anim_select=0;}
            a_path = "ASSETS/CHARA.AST";
            t_path = "ASSETS/CHARA.RPF";
            break;
        }

        case A_TYP_SKELETON :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;out->animate=true;out->anim_select=0;}
            a_path = "ASSETS/SKELE.AST";
            t_path = "ASSETS/SKELE.RPF";
            break;
        }

        case A_TYP_MINION :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;out->animate=true;out->anim_select=1;}
            a_path = "ASSETS/SKELE.AST";
            t_path = "ASSETS/SKELE.RPF";
            break;
        }
        
        default: {
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;out->animate=false;}
            break;
        }
    }
    
    if(out){

        out->asset = NULL;
        out->texture = NULL;
        out->animation_controller = NULL;

        out->type = type;
        out->state = 0;
        out->relative_start_frame = get_frame();

        bool fail = false;
        if(a_path){

            const char * ap = a_path;

            out->asset = load_asset(ap);
            if(!out->asset){
                fail = true;
            }

            else if(out->asset->model&&out->asset->model->skeletal_rig&&out->asset->model->skeletal_rig->joints&&out->asset->model->skeletal_rig->anim_count&&out->asset->model->skeletal_rig->joint_count){
                out->animation_controller = malloc(out->asset->model->skeletal_rig->joint_count*sizeof(ANIM_CONTROLLER));

                if(!out->animation_controller){
                    fail = true;
                }
                else{

                    for (uint8_t k = 0; k < out->asset->model->skeletal_rig->joint_count; k++)
                    {
                        out->animation_controller[k].t_current=0;
                        out->animation_controller[k].r_current=0;
                        out->animation_controller[k].s_current=0;

                    }
                }
            }

        }
        if(t_path && !fail){

            const char * tp = t_path;

            out->texture = load_texture(tp);
            if(!out->texture){
                fail = true;
            }
        }

        if(fail){
            destroy_actor(out);
            out = NULL;           
        }



    }

    return out;
}
