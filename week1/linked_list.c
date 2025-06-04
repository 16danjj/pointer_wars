#include "linked_list.h"
#include <stdlib.h>

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

bool linked_list_register_malloc(void * (*malloc)(size_t)) {
    malloc_fptr = malloc;
    return true;
}

bool linked_list_register_free(void (*free)(void*)) {
    free_fptr = &free;
    return true;
}

struct linked_list * linked_list_create(void) {

    linked_list_register_malloc(&malloc);
    linked_list_register_free(&free);

    struct linked_list *ll = (struct linked_list *)malloc_fptr(sizeof(struct linked_list));

    if (ll != NULL) {
        ll->head = NULL;
    }

    return ll;
}

bool linked_list_delete(struct linked_list * ll) { 
    
    if (ll == NULL) {
        return false;
    }

    struct node *current_node = ll->head;

    if (current_node != NULL) {

        while (current_node != NULL) {
            struct node *next_node = current_node->next;
            free_fptr(current_node);

            current_node = next_node;
        }
    }

    return true;

}

size_t linked_list_size(struct linked_list * ll) {

    size_t count = 0;

    if (ll == NULL) {
        return SIZE_MAX;
    }

    struct node *current_node = ll->head;

    while (current_node != NULL) {
        count += 1; 
        current_node = current_node->next;
    }

    return count;
}