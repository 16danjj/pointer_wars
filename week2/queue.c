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

#include "queue.h"

// Implement your queue functions here.
//


// Function pointers to malloc() and free() functions 
static void * (*malloc_fptr)(size_t size) = NULL;
static void (*free_fptr)(void* addr) = NULL;


// Registers malloc function pointer to use function defined in linked_list_test_program.c
bool queue_register_malloc(void * (*malloc)(size_t)) {
    malloc_fptr = malloc;
    return true;
}

// Registers free function pointer to use function defined in linked_list_test_program.c 
bool queue_register_free(void (*free)(void*)) {
    free_fptr = free;
    return true;
} 

// Creates a new queue.
struct queue * queue_create(void) {

    struct queue *q = (struct queue *)malloc_fptr(sizeof(struct queue));
    linked_list_register_malloc(&malloc);
    linked_list_register_free(&free);

    if (q != NULL) {
        q->ll = linked_list_create();
    }

    return q;
}

// Deletes a queue.
bool queue_delete(struct queue * queue) {

    if (queue == NULL) {
        return false;
    }

    linked_list_delete(queue->ll);
    free_fptr(queue);

    return true;
}

// Pushes an unsigned int onto the queue.
bool queue_push(struct queue * queue, unsigned int data) {

    if (queue == NULL) {
        return false;
    }

    return linked_list_insert_end(queue->ll, data);
}

// Returns the size of the queue.
size_t queue_size(struct queue * queue) {

    if (queue == NULL) {
        return SIZE_MAX;
    }

    return linked_list_size(queue->ll);
}

// Returns whether an entry exists to be popped.
bool queue_has_next(struct queue * queue) {

    size_t index = queue_size(queue);

    if (index == SIZE_MAX || index == 0) {
        return false;
    }

    return true;
}

// Pushes an unsigned int onto the queue.
bool queue_next(struct queue * queue, unsigned int * popped_data) {

    if (!queue_has_next(queue)){
        return false;
    }

    //size_t index = queue_size(queue);

    struct iterator *it = linked_list_create_iterator(queue->ll, 0);

    if (it == NULL) {
        return false;
    } else {
        *popped_data = it->data;
        return true;
    }
}

// Pops an unsigned int from the queue, if one exists.
bool queue_pop(struct queue * queue, unsigned int * popped_data) {

    if (queue_next(queue, popped_data)) {
        linked_list_remove(queue->ll, 0);
        return true;
    }

    return false;
}