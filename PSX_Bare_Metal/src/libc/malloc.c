#include <stdlib.h>

#define ALIGN(addr,N) ((addr + (N-1))&(~(N-1)))

extern uint8_t _bssEnd[];
extern uint8_t _stackStart[];

#define HEAP_START (uintptr_t)&_bssEnd
#define HEAP_LIMIT (uintptr_t)&_stackStart

typedef struct _Block
{
    struct _Block *next;
    struct _Block *prev;
    void *ptr;
    size_t size;     
}Block;

Block *malloc_head;
Block *malloc_tail;

void initHeap(){
    malloc_head = (Block*)(HEAP_START);
    malloc_tail = (Block*)(HEAP_LIMIT-sizeof(Block));

    malloc_head->next=malloc_tail;
    malloc_head->prev=NULL;

    malloc_tail->next=NULL;
    malloc_tail->prev=malloc_head;

    malloc_head->ptr=NULL;
    malloc_tail->ptr=NULL;

    malloc_head->size=(HEAP_LIMIT-HEAP_START-(2*sizeof(Block)));
    malloc_tail->size=0;    
}

void *malloc(size_t size){
    if(size==0){
        return NULL;
    }
    size_t adjusted_size = size + sizeof(Block);
    Block *current = malloc_head;
    while (current!=malloc_tail){
        if(current->ptr==NULL&&current->size>=adjusted_size){
            Block *newBlock = (Block*)((uintptr_t)current->next-adjusted_size);
            newBlock->ptr = (void*)((uintptr_t)newBlock+sizeof(Block));
            
            current->size-=adjusted_size;
            newBlock->size=size;
           
            newBlock->next=current->next;
            newBlock->prev=current;
           
            newBlock->next->prev=newBlock;
            newBlock->prev->next=newBlock;
            return newBlock->ptr;
        }
        current=current->next;
    }       
    return NULL;
}

void free(void *ptr){
    if((uintptr_t)ptr<=(uintptr_t)malloc_head||(uintptr_t)ptr>=(uintptr_t)malloc_tail){
        return;
    }
    Block *owner = (Block*)((uintptr_t)ptr-sizeof(Block));
    owner->ptr=NULL;
    Block *next = owner->next;
    Block *prev = owner->prev;
    
    while (next!=malloc_tail && next->ptr==NULL)
    {
        owner->size+=(next->size+sizeof(Block));
        owner->next=next->next;
        owner->next->prev=owner;
        next=next->next;
    }

    while (prev!=NULL && prev->ptr==NULL)
    {
        prev->size+=(owner->size+sizeof(Block));
        prev->next=owner->next;
        owner=prev;
    }
    
    
}

void *realloc(void *ptr, size_t size){
    if(size==0||(uintptr_t)ptr<=(uintptr_t)malloc_head||(uintptr_t)ptr>=(uintptr_t)malloc_tail){
        free(ptr);
        return NULL;
    } 
    size_t adjusted_size = size + sizeof(Block);
    Block *owner = (Block*)((uintptr_t)ptr-sizeof(Block));
    if(size<owner->size){
        void *new_ptr = malloc(size);
        if(new_ptr!=NULL){
            memcpy(new_ptr,ptr,size);
            free(ptr);
            return new_ptr;
        }
        else{
            new_ptr = (void*)((uintptr_t)owner+(size-owner->size));
            owner->prev->next = (Block*)new_ptr;
            owner->next->prev = (Block*)new_ptr;
            owner->prev->size+=(size-owner->size);
            owner->size-=(size-owner->size);            
            return (void*)((uintptr_t)memmove(new_ptr,(void*)owner,adjusted_size)+sizeof(Block));
        }
    }
    else if(size>owner->size){
        if(owner->prev->ptr==NULL&&owner->prev->size>=(size-owner->size)){
            owner->prev->size-=(size-owner->size);
            owner->size+=(size-owner->size);
            owner->ptr=(void*)((uintptr_t)owner->ptr-(size-owner->size));
            void *new_ptr = (void*)((uintptr_t)owner->ptr-sizeof(Block)); 
            owner->prev->next = (Block*)new_ptr;
            owner->next->prev = (Block*)new_ptr;
            return (void*)((uintptr_t)memcpy(new_ptr,(void*)owner,adjusted_size)+sizeof(Block));
        }
        else{
            void *new_ptr = malloc(size);
            if(new_ptr==NULL){
                return NULL;
            }
            memcpy(new_ptr,ptr,owner->size);
            free(ptr);
            return new_ptr; 
        }
    }    
    else{
        return ptr;
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