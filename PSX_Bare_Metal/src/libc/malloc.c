#include <stdlib.h>
#include <interrupts.h>
#define ALIGN(addr,N) ((addr + (N-1))&(~(N-1)))

extern uint8_t _bssEnd[];
extern uint8_t _stackStart[];

#define HEAP_START (uintptr_t)_bssEnd
#define HEAP_LIMIT (uintptr_t)_stackStart

typedef struct _Block {
	struct _Block *next;
    struct _Block *prev;
	void   *ptr;
	size_t size;
}Block;

static Block* malloc_head = NULL;

static inline void init_heap(){
    malloc_head = (Block*)ALIGN(HEAP_START,8);
    malloc_head->ptr=NULL;
    malloc_head->next=NULL;
    malloc_head->prev=NULL;
    malloc_head->size=ALIGN(HEAP_LIMIT-(uintptr_t)&malloc_head[1],8);
}

void *malloc(size_t size){   
    enter_crit_section();
    if(!malloc_head){
        init_heap();
    }    
    if(!size){
        exit_crit_section();
        return NULL;
    }
    size_t adjusted_size = ALIGN(size+sizeof(Block),8);    
    Block *current = malloc_head;
    while (current) 
    {
        if(!current->ptr && current->size - (adjusted_size-sizeof(Block))<sizeof(Block)){
            current->ptr=&current[1];
            current->size=adjusted_size-sizeof(Block);
            exit_crit_section();
            return current->ptr;
        }
        else if(!current->ptr && current->size>=adjusted_size){
            Block *free_space = (Block*)((uintptr_t)current+adjusted_size);
            free_space->size=current->size-adjusted_size;
            free_space->ptr=NULL;
            free_space->next=current->next;
            current->ptr=&current[1];
            current->size=adjusted_size-sizeof(Block);
            current->next = free_space;
            free_space->prev = current;
            exit_crit_section();
            return current->ptr;
        }
        current=current->next;
    }
    exit_crit_section();
    return NULL;
}

void free(void *ptr){
    enter_crit_section();
    if(!malloc_head){
        init_heap();
    } 
    if(!ptr){
        exit_crit_section();
        return;   
    }
    Block *owner = ((Block*)ptr) - 1;
    owner->ptr=NULL;
    Block *eval = owner->next;
    while (eval && !eval->ptr)
    {
        owner->next=eval->next;
        if(owner->next){
            owner->next->prev=owner;
        }
        owner->size+=(eval->size+sizeof(Block));
        eval=eval->next;
    }
    eval=owner->prev;
    while (eval && !eval->ptr)
    {   
        eval->next=owner->next;
        if(owner->next){
            owner->next->prev=eval;
        }
        eval->size+=(owner->size+sizeof(Block));
        owner=eval;
        eval=eval->prev;        
    } 
    exit_crit_section();
}


void *realloc(void *ptr, size_t size){
    enter_crit_section();
    if(!malloc_head){
        init_heap();
    } 
    if(!size){
        free(ptr);
        exit_crit_section();
        return NULL;
    }
    if(!ptr){
        exit_crit_section();
        return malloc(size);
    }
    Block *owner = ((Block*)ptr) - 1;
    size_t adjusted_size = ALIGN(size+sizeof(Block),8);    
    Block *new_block;
    if(owner->size==adjusted_size-sizeof(Block)){
        exit_crit_section();
        return ptr;
    }
    else if(adjusted_size-sizeof(Block)>owner->size){
        if(owner->next&&!owner->next->ptr&&(owner->size+owner->next->size+sizeof(Block)>=adjusted_size)){
            new_block = (Block*)((uintptr_t)owner+adjusted_size-sizeof(Block));
            new_block->size=owner->size+owner->next->size+sizeof(Block)-adjusted_size;
            new_block->ptr=NULL;
            new_block->prev=owner;
            new_block->next=owner->next->next;
            if(new_block->next){
                new_block->next->prev=new_block;
            }
            owner->next=new_block;
            owner->size=size;
            exit_crit_section();
            return owner->ptr;
        }
        else{
            new_block = malloc(size);
            if(!new_block){
                exit_crit_section();
                return NULL;
            }
            memcpy(new_block->ptr,owner->ptr,owner->size);
            free(ptr);
            exit_crit_section();
            return new_block->ptr;
        }
    }
    else{
        if(owner->size>=adjusted_size){
            new_block = (Block*)((uintptr_t)owner+adjusted_size);
            new_block->size=owner->size-adjusted_size;
            new_block->ptr=NULL;
            new_block->next=owner->next;
            if(new_block->next){
                new_block->next->prev=new_block;
            }
            new_block->prev=owner;
            owner->next=new_block;
            owner->size=size;
            exit_crit_section();
            return owner->ptr;
        }
        else{
            new_block = malloc(size);
            if(!new_block){
                exit_crit_section();
                return NULL;
            }
            memcpy(new_block->ptr,owner->ptr,size);
            free(ptr);
            exit_crit_section();
            return new_block->ptr;
        }
    }
}

void *memcpy(void* dest, const void* src, size_t len){
    if(len!=0&&dest!=src){
        uint8_t *destination = (uint8_t*)dest;
        const uint8_t *source = (uint8_t*)src;
        for (size_t i = 0; i < len; i++){
            destination[i]=source[i];
        }
    }   
    return dest;
}

void *memmove(void* dest, const void* src, size_t len){
    if(len!=0&&dest!=src){
        uint8_t *destination = (uint8_t*)dest;
        const uint8_t *source = (uint8_t*)src;
        if(dest<src){
            for (size_t i = 0; i < len; i++){
                destination[i]=source[i];
            }            
        }
        else{
             for (size_t i = len; i > 0; i--){
                destination[i-1]=source[i-1];
            }  
        }       
    }   
    return dest;
}