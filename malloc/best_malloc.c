//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Interfaces to get memory pages from OS
//

void *mmap_from_system(size_t size);
void munmap_to_system(void *ptr, size_t size);

//
// Struct definitions
//

typedef struct my_metadata_t {
  size_t size;
  struct my_metadata_t *next;
} my_metadata_t;

typedef struct my_heap_t {
  my_metadata_t *free_head;
  my_metadata_t dummy;
} my_heap_t;

//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap;

//
// Helper functions (feel free to add/remove/edit!)
//



void my_remove_from_free_list(my_metadata_t *metadata, my_metadata_t *prev) {
  if (prev) {
    prev->next = metadata->next;
  } else {
    my_heap.free_head = metadata->next;
  }
  metadata->next = NULL;
}

void my_add_to_free_list(my_metadata_t *metadata) {
    my_metadata_t *curr = my_heap.free_head;
    my_metadata_t *prev = NULL;

    // Immediate Coalescing
    if ((char*)metadata + metadata->size == (char*)curr) {
        metadata->size += curr->size + sizeof(my_metadata_t);
        my_remove_from_free_list(curr, prev);
    }

    curr = my_heap.free_head;
    prev = NULL;

    // Sorted Free List (insertion in ascending order of size)
    while (curr && curr->size < metadata->size) {
        prev = curr;
        curr = curr->next;
    }

    metadata->next = curr;
    if (prev) {
        prev->next = metadata;
    } else {
        my_heap.free_head = metadata;
    }
}

//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
void my_initialize() {
  my_heap.free_head = &my_heap.dummy;
  my_heap.dummy.size = 0;
  my_heap.dummy.next = NULL;
}

// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().
void *my_malloc(size_t size) {
  my_metadata_t *metadata = my_heap.free_head;
  my_metadata_t *prev = NULL;
  my_metadata_t *best_fit = NULL;
  my_metadata_t *best_fit_prev = NULL;

  while (metadata) {
    if (metadata->size >= size && (!best_fit || metadata->size < best_fit->size)) {
      best_fit = metadata;
      best_fit_prev = prev;
    }
    prev = metadata;
    metadata = metadata->next;
  }

  if (!best_fit) {
    size_t buffer_size = 4096;
    my_metadata_t *metadata = (my_metadata_t *)mmap_from_system(buffer_size);
    metadata->size = buffer_size - sizeof(my_metadata_t);
    metadata->next = NULL;
    my_add_to_free_list(metadata);
    return my_malloc(size);
  } else {
    metadata = best_fit;
    prev = best_fit_prev;
  }

  void *ptr = metadata + 1;
  size_t remaining_size = metadata->size - size;
  my_remove_from_free_list(metadata, prev);

  if (remaining_size > sizeof(my_metadata_t)) {
    metadata->size = size;
    my_metadata_t *new_metadata = (my_metadata_t *)((char *)ptr + size);
    new_metadata->size = remaining_size - sizeof(my_metadata_t);
    new_metadata->next = NULL;
    my_add_to_free_list(new_metadata);
  }
  return ptr;
}

// This is called every time an object is freed.  You are not allowed to
// use any library functions other than mmap_from_system / munmap_to_system.
void my_free(void *ptr) {
  // Look up the metadata. The metadata is placed just prior to the object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  my_metadata_t *metadata = (my_metadata_t *)ptr - 1;
  // Add the free slot to the free list.
  my_add_to_free_list(metadata);
}

// This is called at the end of each challenge.
void my_finalize() {
  // Nothing is here for now.
  // feel free to add something if you want!
}

void test() {
  // Implement here!
  assert(1 == 1); /* 1 is 1. That's always true! (You can remove this.) */
}
