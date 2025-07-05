/*

MIT License

Copyright (c) 2025 Dan Jose

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "linked_list.h"

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 
static struct free_list f_list = {.head = NULL, .size = 0, .allocated = 0, .next_to_allocate = NULL};

static struct node * __linked_list_create_node() {

    if (f_list.head == NULL){
        struct free_node *f = malloc_fptr(1000 * sizeof(struct free_node));
        struct free_node *curr = NULL;

        for (int i = 0; i<1000; i++) {
            if (i == 0) {
                f_list.head = &f[i];
                curr = f_list.head;
                continue;
            }
            curr->next = &f[i];
            curr = curr->next;
        }

        f_list.size += 1000;
        f_list.allocated += 1;
        f_list.next_to_allocate = f_list.head->next;

        f_list.head->node_ptr = (struct node*)malloc_fptr(sizeof(struct node));
        return f_list.head->node_ptr;

    } else if (f_list.allocated == f_list.size - 1) {
        struct free_node *f = malloc_fptr(1000 * sizeof(struct free_node));
        struct free_node *curr = NULL;

        for (int i = 0; i<1000; i++) {
            if (i == 0) {
                f_list.next_to_allocate = &f[i];
                curr = &f[i];
                continue;
            }

            curr->next = &f[i];
            curr = curr->next;
        }
    }

    f_list.allocated += 1;
    struct free_node *curr = f_list.next_to_allocate;
    f_list.next_to_allocate = curr->next;

    curr->node_ptr = (struct node *)malloc_fptr(sizeof(struct node));
    return curr->node_ptr;
}


static void __linked_list_delete_node() {

    size_t allocated = f_list.allocated;
    size_t count = 0;
    struct free_node *prev = f_list.head;
    struct free_node *curr = f_list.head;

    while (count < allocated) {
        prev = curr;
        curr = curr->next;
        count += 1;
    }

    assert(curr == f_list.next_to_allocate);
    f_list.allocated -= 1;
    f_list.next_to_allocate = prev;
}

void linked_list_final_cleanup() {
    struct free_node *curr = f_list.head;
    struct free_node *next_node = f_list.head;

    while (curr != NULL) {
        free_fptr(curr->node_ptr);
        next_node = next_node->next;
        curr = next_node;
        f_list.size -= 1;
    }

    assert(f_list.size == 0);

    curr = f_list.head;
    free_fptr(curr);
    f_list.allocated = 0;
    f_list.head = NULL;
    f_list.next_to_allocate = NULL;
}


// Populate all fields of an iterator to the beginning of a linked list
static void __linked_list_populate_iterator(struct linked_list * ll, struct iterator * iter) {

    iter->ll = ll;
    iter->current_index = 0;
    iter->current_node = ll->head;
    iter->data = ll->head->data;
}

// Registers malloc function pointer to use function defined in linked_list_test_program.c 
bool linked_list_register_malloc(void * (*malloc)(size_t)) {
    malloc_fptr = malloc;
    return true;
}

// Registers free function pointer to use function defined in linked_list_test_program.c 
bool linked_list_register_free(void (*free)(void*)) {
    free_fptr = free;
    return true;
} 

// Creates a new linked list
struct linked_list * linked_list_create(void) {

    struct linked_list *ll = (struct linked_list *)malloc_fptr(sizeof(struct linked_list));

    if (ll != NULL) {
        ll->head = NULL;
        ll->tail = NULL;
        ll->size = 0;
    }

    return ll;
}

// Deletes a linked list, also makes sure all nodes are properly freed before list deletion
bool linked_list_delete(struct linked_list * ll) { 
    
    if (ll == NULL) {
        return false;
    }

    struct node *current_node = ll->head;

    if (current_node != NULL) {

        while (current_node != NULL) {
            struct node *next_node = current_node->next;
            __linked_list_delete_node();
            ll->size -= 1;

            current_node = next_node;
        }
    }

    free_fptr(ll);
    return true;

}

// Returns current size of the linked list
size_t linked_list_size(struct linked_list * ll) {

    if (ll == NULL) {
        return SIZE_MAX;
    }

    return ll->size;
}

// Creates a new node to insert, and inserts at end of linked list
bool linked_list_insert_end(struct linked_list * ll, unsigned int data) {

    if (ll == NULL) {
        return false;
    }

    struct node *node_to_insert = __linked_list_create_node();

    if (node_to_insert == NULL) {
        return false;
    }
    node_to_insert->data = data;
    node_to_insert->next = NULL;

    struct node *current_node = ll->tail;

    if (current_node == NULL) {
        ll->head = node_to_insert;
        ll->tail = node_to_insert;
    } else {
        current_node->next = node_to_insert;
        ll->tail = node_to_insert;
    }

    ll->size += 1;
    return true;
}

// Creates a new node to insert, and inserts at front of linked list
bool linked_list_insert_front(struct linked_list * ll, unsigned int data) {

    if (ll == NULL) {
        return false;
    }

    struct node *node_to_insert = __linked_list_create_node();

    if (node_to_insert == NULL) {
        return false;
    }

    node_to_insert->data = data;
    node_to_insert->next = NULL;

    struct node *current_node = ll->head;

    if (current_node == NULL) {
        ll->head = node_to_insert;
        ll->tail = node_to_insert;
    } else {
        node_to_insert->next = current_node;
        ll->head = node_to_insert;
    }

    ll->size += 1;
    return true;

}

// Creates a new node to insert at a particular index in linked list
bool linked_list_insert(struct linked_list * ll, size_t index, unsigned int data) {

    if (index == 0) {
        return linked_list_insert_front(ll, data);
    } else if (ll != NULL && ll->size == index) {
        return linked_list_insert_end(ll, data);
    }

    if (ll == NULL || ll->head == NULL) {
        return false;
    }

    struct node *node_to_insert = __linked_list_create_node();

    if (node_to_insert == NULL) {
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

    ll->size += 1;
    return true;
}

// Returns index of matching node based on data field
// Function has been modified to make use of an iterator for better performance
size_t linked_list_find(struct linked_list * ll, unsigned int data) {

    if (ll == NULL) {
        return SIZE_MAX;
    }

    struct iterator iter;
    __linked_list_populate_iterator(ll, &iter);

    if (iter.current_node == NULL) {
        return SIZE_MAX;
    }

    while (iter.current_node != NULL) {

        if (iter.data == data) {
            return iter.current_index;
        }
        
        if (!linked_list_iterate(&iter)) {
            break;
        }
    }

    return SIZE_MAX;
}

// Removes node specified at index from linked list
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
        __linked_list_delete_node();
        ll->size -= 1;

        if (ll->size == 0) {
            ll->tail = NULL;
        }
        return true;
    }

    while (count != index) {
        prev_node = current_node;
        current_node = current_node->next;
        count += 1;
    }

    prev_node->next = current_node->next;
    if (current_node == ll->tail) {
        ll->tail = prev_node;
    }
    __linked_list_delete_node();
    ll->size -= 1;

    return true;
}

// Creates an iterator over the linked list
struct iterator * linked_list_create_iterator(struct linked_list * ll, size_t index) {

    if (ll == NULL || ll->head == NULL) {
        return NULL;
    }

    struct iterator *it = (struct iterator *)malloc_fptr(sizeof(struct iterator));

    if (it == NULL) {
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

// Deletes iterator over the linked list
bool linked_list_delete_iterator(struct iterator * iter) {

    if (iter == NULL) {
        return false;
    }

    free_fptr(iter);
    return true;
}

// Iterator stores node information of next node in linked list
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