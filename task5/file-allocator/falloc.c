#include <string.h>
#include "falloc.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <stdbool.h>
#include <fcntl.h>

void falloc_init(file_allocator_t* allocator, const char* filepath, uint64_t allowed_page_count) {
    int opening_fd;
    int trunk_to = (allowed_page_count + 1) * PAGE_SIZE;
    auto masking = 0b1;
    if ((opening_fd = open(filepath, O_CREAT | O_RDWR, 0664)) < 0) {
        perror("Error while opening a file\n");
        return;
    }
    void *map_trial = mmap(NULL, trunk_to, PROT_WRITE, MAP_SHARED, opening_fd, 0);
    if (map_trial == MAP_FAILED) {
        perror("Error while mmaping\n");
        return;
    }
    uint64_t page_counter = 0; 
    ftruncate(opening_fd, trunk_to);
    allocator->base_addr = map_trial + PAGE_SIZE;
    allocator->page_mask = (uint64_t*)map_trial;
    allocator->fd = opening_fd;
    allocator->allowed_page_count = allowed_page_count;
    for (int i = 0; i < PAGE_SIZE; ++i) {
        if (masking & (allocator->page_mask[i / 64] >> (i % 64))) {
            ++page_counter;
        }
    }
    allocator->curr_page_count = page_counter;
}
void* falloc_acquire_page(file_allocator_t* allocator) {
    bool this_bit = 1;
    uint64_t i = 0;
    if (allocator->curr_page_count == allocator->allowed_page_count) {
        return NULL;
    }
    uint64_t* alloc_mask = allocator->page_mask;
    while (this_bit) {
        uint64_t cur_set = alloc_mask[i / 64];
        this_bit = 1 & (cur_set >> (i % 64));
        if (!this_bit) {
            alloc_mask[i / 64] |= (1UL << (i % 64));
            break;
        }
        ++i;
    }
    ++allocator->curr_page_count;
    auto offset = i * PAGE_SIZE;
    return allocator->base_addr + offset;
}
void falloc_release_page(file_allocator_t* allocator, void** addr) {
    auto masking = 0b1;
    for (int i = 0; i < allocator->allowed_page_count; ++i) {
        if (allocator->base_addr + i * PAGE_SIZE == *addr) {
            *addr = NULL;
            auto should_not = (masking << (i % 64));
            allocator->page_mask[i / 64] ^= should_not;
            --allocator->curr_page_count;
            return;
        }
    }
}
void falloc_destroy(file_allocator_t* allocator) {
    if (close(allocator->fd) < 0) {
        perror("Error while closing file\n");
        return;
    }
    auto max_amount = (allocator->allowed_page_count + 1) * PAGE_SIZE;
    if (munmap(allocator->page_mask, max_amount) < 0) {
        perror("Error in unmap\n");
        return;
    }
    allocator->base_addr = NULL;
    allocator->curr_page_count = 0;
    allocator->page_mask = NULL;
}