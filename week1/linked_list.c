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
    free_fptr = free;
    return true;
} 

struct linked_list * linked_list_create(void) {

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


bool linked_list_insert_end(struct linked_list * ll, unsigned int data) {

    struct node *node_to_insert = (struct node *)malloc_fptr(sizeof(struct node));

    if (node_to_insert == NULL || ll == NULL) {
        return false;
    }
    node_to_insert->data = data;
    node_to_insert->next = NULL;

    struct node *current_node = ll->head;

    if (current_node == NULL) {
        ll->head = node_to_insert;

        return true;
    }

    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    current_node->next = node_to_insert;

    return true;
}

bool linked_list_insert_front(struct linked_list * ll, unsigned int data) {

    struct node *node_to_insert = (struct node *)malloc_fptr(sizeof(struct node));

    if (node_to_insert == NULL || ll == NULL) {
        return false;
    }
    node_to_insert->data = data;
    node_to_insert->next = NULL;

    struct node *current_node = ll->head;

    if (current_node == NULL) {
        ll->head = node_to_insert;
    } else {
        node_to_insert->next = current_node;
        ll->head = node_to_insert;
    }

    return true;

}

bool linked_list_insert(struct linked_list * ll, size_t index, unsigned int data) {

    if (index == 0) {
        return linked_list_insert_front(ll, data);
    }

    struct node *node_to_insert = (struct node *)malloc_fptr(sizeof(struct node));

    if (node_to_insert == NULL || ll == NULL || ll->head == NULL) {
        return false;
    }
    node_to_insert->data = data;
    node_to_insert->next = NULL;

    struct node *prev_node = ll->head;
    struct node *next_node = prev_node;
    size_t count = 0;

    while (count != index) {
        prev_node = next_node;
        next_node = next_node->next;
        count += 1;
    }

    prev_node->next = node_to_insert;
    node_to_insert->next = next_node;

    return true;
}

size_t linked_list_find(struct linked_list * ll, unsigned int data) {

    if (ll == NULL) {
        return SIZE_MAX;
    }

    struct node *current_node = ll->head;
    size_t index = 0;

    if (current_node == NULL) {
        return SIZE_MAX;
    }

    while (current_node != NULL) {

        if (current_node->data == data) {
            return index;
        }

        index += 1;
        current_node = current_node->next;
    }

    return SIZE_MAX;
}

bool linked_list_remove(struct linked_list * ll, size_t index) {

    if (ll == NULL) {
        return false;
    }

    struct node *current_node = ll->head;
    struct node *prev_node = ll->head;
    size_t count = 0;

    if (current_node == NULL) {
        return false;
    }

    if (index == 0) {
        ll->head = current_node->next;
        free_fptr(current_node);
        return true;
    }

    while (count != index) {
        prev_node = current_node;
        current_node = current_node->next;
        count += 1;
    }

    prev_node->next = current_node->next;
    free_fptr(current_node);

    return true;
}

struct iterator * linked_list_create_iterator(struct linked_list * ll, size_t index) {

    struct iterator *it = (struct iterator *)malloc_fptr(sizeof(struct iterator));

    if (it == NULL || ll == NULL || ll->head == NULL) {
        return NULL;
    }

    size_t count = 0;
    struct node *current_node = ll->head;

    while (count != index) {
        current_node = current_node->next;
        count += 1;
    }

    it->ll = ll;
    it->current_index = count;
    it->current_node = current_node;
    it->data = current_node->data;

    return it;
}


bool linked_list_delete_iterator(struct iterator * iter) {

    if (iter == NULL) {
        return false;
    }

    free_fptr(iter);
    return true;
}


bool linked_list_iterate(struct iterator * iter) {
    if (iter == NULL || iter->current_node == NULL) {
        return false; 
    }

    struct node *next_node = iter->current_node->next;

    if (next_node == NULL) {
        return false;
    }

    size_t new_index = iter->current_index + 1;

    iter->current_index = new_index;
    iter->current_node = next_node;
    iter->data = next_node->data;

    return true;
} 