#include <stdlib.h>

#define ALIGN(a,b) (((a)+((b)-1))&(~((b)-1)))

extern uint8_t _bssEnd[];
extern uint8_t _stackStart[];

static uintptr_t heapEnd = (uintptr_t) & _bssEnd;
static uintptr_t heapLimit = (uintptr_t) & _stackStart;

void * sbrk(ptrdiff_t increment){

    uintptr_t current = heapEnd;
    uintptr_t candidate = ALIGN((heapEnd+increment),8);

    if(candidate>=heapLimit){
        return NULL;
    }

    else{
        heapEnd = candidate;
        return (void*) current;
    }
  
}

typedef struct _Block
{
    struct _Block* previous;
    struct _Block* next;
    void * ptr;
    size_t size;

}Block;

static void  *mallocStart;
static Block *mallocHead;
static Block *mallocTail;

static Block *_findBlock(Block* head, size_t size){
    Block *current = head;
    
    while (current)
    {
        if(current->next){

            uintptr_t temp = (uintptr_t) current->next;
            temp -= (uintptr_t)current->ptr + current->size;

            if(temp>=size){
                return current;
            }
        }

        current = current->next;
    }   

    return current;
}

void * malloc(size_t size){
    
    if (!size)
    {
        return NULL;
    }
    
    else{

        Block *newBlock;
        Block *prevBlock;
        size_t _size = ALIGN(size+sizeof(Block),8);
        void *ptr;
     
        if(!mallocHead){
            if(!mallocStart){
                mallocStart = (Block*)sbrk(0);
            }
            newBlock = (Block*)sbrk(_size);
            if(!newBlock){
                return NULL;
            }
            else{

                ptr = (void*) &newBlock[1];
                newBlock->ptr=ptr;
                newBlock->size=_size-sizeof(Block);
                newBlock->previous=NULL;
                newBlock->next=NULL;

                mallocHead=newBlock;
                mallocTail=newBlock;

                return ptr;

            }
        }

        else if(((uintptr_t)mallocStart+_size)<(uintptr_t)mallocHead){
            newBlock = (Block*)mallocStart;
        
            ptr = (void*) &newBlock[1];
            newBlock->ptr=ptr;
            newBlock->size=_size-sizeof(Block);
            newBlock->previous=NULL;
            newBlock->next=mallocHead;

            mallocHead->previous=newBlock;
            mallocHead=newBlock;

            return ptr;

        }

        else{

            prevBlock = _findBlock(mallocHead,_size);

            if(prevBlock){
                
                newBlock =(Block*) ((uintptr_t)prevBlock->ptr+prevBlock->size);
                ptr = (void*)((uintptr_t)newBlock+sizeof(Block));
                newBlock->ptr=ptr;
                newBlock->size=_size-sizeof(Block);
                newBlock->previous=prevBlock;
                newBlock->next=prevBlock->next;

                (newBlock->previous)->next=newBlock;
                prevBlock->next = newBlock;

            }

            else{

                newBlock = (Block*) sbrk(_size);
                if(!newBlock){
                    return NULL;
                }

                ptr = (void*)&newBlock[1];
                newBlock->ptr = ptr;
                newBlock->size = _size - sizeof(Block);
                newBlock->previous=mallocTail;
                newBlock->next=NULL;

                mallocTail->next = newBlock;
                mallocTail=newBlock;
            
            }

            return ptr;
        }

    }

}

void * realloc(void *ptr, size_t size){
    if(!size){
        free(ptr);
        return NULL;
    }

    else if(!ptr){
        return malloc(size);
    }

    else{

        void *new_ptr;

        size_t _size = ALIGN(size+sizeof(Block),8);
        Block *previousBlock = (Block*) ((uintptr_t)ptr-sizeof(Block));

        if(previousBlock->size>=_size){
            previousBlock->size=_size;
            if(previousBlock->next){
                sbrk((ptr-sbrk(0))+_size);
            }
            return ptr;
        }

        else if(!previousBlock->next){
            new_ptr = sbrk(_size-previousBlock->size);
            if(!new_ptr){
                return NULL;
            }
            else{
                previousBlock->size = _size;
                return ptr;
            }
        }

        else if(((previousBlock->next)->ptr-ptr)>_size){
            previousBlock->size=_size;
            return ptr;
        }

        else{
            new_ptr=malloc(size);
            if(!new_ptr){
                return NULL;
            }
            else{
                __builtin_memcpy(new_ptr,ptr,previousBlock->size);
                free(ptr);
                return new_ptr;
            }
            
        }

    }

}

void free(void *ptr){

    if(!ptr||!mallocHead){
        return;
    }

    else if(mallocHead->ptr==ptr){

        size_t _size = mallocHead->size;
        _size+=((uintptr_t)mallocHead->ptr-(uintptr_t)mallocHead);
        mallocHead=mallocHead->next;

        if(mallocHead){
            mallocHead->previous=NULL;
        }
        else{
            mallocTail=NULL;
            sbrk(-_size);
        }

        return;
    }

    else{

        Block *current;

        for (current =mallocHead;current->ptr!=ptr;current=current->next)
        {
           if(!current->next){
            return;
           }
        }
        
        if(current->next){
            (current->next)->previous=current->previous;
        }

        else{

            void *top = sbrk(0);
            size_t new_size = (top-(current->previous)->ptr)-(current->previous)->size;
            mallocTail=current->previous;

            sbrk(-new_size);
        }

        (current->previous)->next=current->next;
    }

}