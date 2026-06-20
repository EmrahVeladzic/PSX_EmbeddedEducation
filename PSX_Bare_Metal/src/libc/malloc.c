#include <stdlib.h>
#include <string.h>
#include <interrupts.h>
#define ALIGN(addr,N) ((addr + (N-1))&(~(N-1)))

extern uint8_t _heapStart[];
extern uint8_t _heapEnd[];

#define HEAP_START (uintptr_t)_heapStart
#define HEAP_LIMIT (uintptr_t)_heapEnd

typedef struct _Block {
	struct _Block *next;
    struct _Block *prev;
	void   *ptr;
	size_t size;
}Block;

static Block* malloc_head = NULL;

static inline void init_heap(){
    if (malloc_head != NULL) {return;}
    malloc_head = (Block*)ALIGN(HEAP_START,8);
    malloc_head->ptr=NULL;
    malloc_head->next=NULL;
    malloc_head->prev=NULL;
    malloc_head->size=(HEAP_LIMIT-((uintptr_t)malloc_head+sizeof(Block)));
}

static void *_malloc_internal(size_t size){
    if(!malloc_head){
        init_heap();
    }    
    if(!size){
        return NULL;
    }
    size_t adjusted_size = ALIGN(size+sizeof(Block),8);    
    Block *current = malloc_head;
    while (current) 
    {
        if(!current->ptr && current->size>=(adjusted_size-sizeof(Block)) && current->size < adjusted_size){
            current->ptr=&current[1];
            return current->ptr;
        }
        else if(!current->ptr && current->size>=adjusted_size){
            Block *free_space = (Block*)((uintptr_t)current+adjusted_size);
            free_space->size=current->size-adjusted_size;
            free_space->ptr=NULL;
            free_space->next=current->next;
            if(free_space->next){
                free_space->next->prev=free_space;
            }
            current->ptr=&current[1];
            current->size=adjusted_size-sizeof(Block);
            current->next = free_space;
            free_space->prev = current;

            return current->ptr;
        }
        current=current->next;
    }
    return NULL;
}

static void _free_internal(void *ptr){
    if(!malloc_head){
        init_heap();
    } 
    if(!ptr){

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
}

static void *_realloc_internal(void *ptr, size_t size){
if(!malloc_head){
        init_heap();
    } 
    if(!size){
        _free_internal(ptr);
        return NULL;
    }
    if(!ptr){
        return _malloc_internal(size);
    }
    Block *owner = ((Block*)ptr) - 1;
    size_t adjusted_size = ALIGN(size+sizeof(Block),8);    
    Block *new_block;
    if(owner->size==adjusted_size-sizeof(Block)){
        return ptr;
    }
    else if(adjusted_size-sizeof(Block)>owner->size){
        if(owner->next&&!owner->next->ptr&&(owner->size+sizeof(Block)+owner->next->size>=adjusted_size)){
            new_block = (Block*)((uintptr_t)owner+adjusted_size);
            new_block->size=(owner->size+sizeof(Block)+owner->next->size)-adjusted_size;
            new_block->ptr=NULL;
            new_block->prev=owner;
            new_block->next=owner->next->next;
            if(new_block->next){
                new_block->next->prev=new_block;
            }
            owner->next=new_block;
            owner->size=adjusted_size-sizeof(Block);
            return owner->ptr;
        }
        else{
            void *new_ptr = _malloc_internal(size);
            if(new_ptr){
                memcpy(new_ptr, owner->ptr, owner->size);
            }
            _free_internal(ptr);
            return new_ptr;
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
            owner->size=adjusted_size-sizeof(Block);
            return owner->ptr;
        }
        else{
            void *new_ptr = _malloc_internal(size);
            if(new_ptr){
                memcpy(new_ptr, owner->ptr, size);
            }
            _free_internal(ptr);
            return new_ptr;
        }
    }
}

void *malloc(size_t size){   
    enter_crit_section();
    void *result = _malloc_internal(size);
    exit_crit_section();
    return result;
}

void free(void *ptr){
    enter_crit_section();
    _free_internal(ptr);
    exit_crit_section();
}


void *realloc(void *ptr, size_t size){
    enter_crit_section();
    void *result = _realloc_internal(ptr,size);
    exit_crit_section();
    return result;
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