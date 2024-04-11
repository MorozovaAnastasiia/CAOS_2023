#include "bloom_filter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
    uint64_t my_hash = 0;
    uint64_t k = 1;
    for (uint64_t i = 0; i < strlen(str); ++i) {
        my_hash = (my_hash + str[i] * k) %  modulus;
        k *= seed;
    }
    return my_hash;
}
// bl
void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count) {
    // TODO: Initialize bloom filter
    // TODO RESIZE
    int64_t sizeof_t = (set_size + 63) / 64;
    bloom_filter->set = (uint64_t*)calloc(sizeof_t, sizeof(uint64_t));
    bloom_filter->set_size = set_size;
    bloom_filter->hash_fn = hash_fn;
    bloom_filter->hash_fn_count = hash_fn_count;
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
    // TODO: Free memory if needed
    free(bloom_filter->set);
    bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
    // TODO: Insert key into set
    
    for (int64_t seed = 1; seed <= bloom_filter->hash_fn_count; ++seed) {
        int64_t curhash = calc_hash(key, bloom_filter->set_size, seed);
        int64_t mask = 1ul << (curhash % 64);
        bloom_filter->set[curhash / 64] |= mask;
        //printf("%ld\n", curhash);
    }

}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
    // TODO: Check if key exists in set
    for (int64_t seed = 1; seed <= bloom_filter->hash_fn_count; ++seed) {
        int64_t curhash = calc_hash(key, bloom_filter->set_size, seed);
        if (!(1 & (bloom_filter->set[curhash / 64] >> (curhash % 64)))){
            return false;
        }
    }
    return true;
}
