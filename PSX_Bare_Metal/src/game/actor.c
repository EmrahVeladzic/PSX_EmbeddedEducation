#include <actor.h>


ACTOR *create_specific_actor(uint32_t type){

    ACTOR *out = NULL;

    char * a_path = NULL;
    char * t_path = NULL;

    switch (type)
    {
        case A_TYP_RAVEN :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;}
            a_path = "ASSETS/RAVEN.AST";
            t_path = "ASSETS/RAVEN.RPF";
            break;
        }

        case A_TYP_PLAYER :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;}
            a_path = "ASSETS/CHARA.AST";
            t_path = "ASSETS/CHARA.RPF";
            break;
        }

        case A_TYP_SKELETON :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;}
            a_path = "ASSETS/SKELE.AST";
            t_path = "ASSETS/SKELE.RPF";
            break;
        }

        case A_TYP_MINION :{
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;}
            a_path = "ASSETS/SKELE.AST";
            t_path = "ASSETS/SKELE.RPF";
            break;
        }
        
        default: {
            out = malloc(sizeof(ACTOR));
            if(out){out->tick=NULL;}
            break;
        }
    }
    
    if(out){

        out->type = type;
        out->state = 0;

        bool fail = false;
        if(a_path){

            const char * ap = a_path;

            out->asset = load_asset(ap);
            if(!out->asset){
                fail = true;
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
