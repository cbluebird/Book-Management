//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/filter/filter.h"
#include <cstdint>
#include <iostream>

// MurmurHash3
uint32_t Filter::murmur3(const void* key, size_t len, uint32_t seed = 100) {
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;

    uint32_t hash = seed;

    const int nblocks = len / 4;
    const uint32_t* blocks = static_cast<const uint32_t*>(key);
    for (int i = 0; i < nblocks; ++i) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t* tail = static_cast<const uint8_t*>(key) + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];

            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}


void Filter::set(int id) {
    size_t hash1 = murmur3(&id, sizeof(id));
    size_t hash2 = std::hash<int>{}(id);

    filter[hash1 % FILTER_SIZE] = 1;
    filter[hash2 % FILTER_SIZE] = 1;
}

bool Filter::get(int id) {
    size_t hash1 = murmur3(&id, sizeof(id));
    size_t hash2 = std::hash<int>{}(id);

    return filter[hash1 % FILTER_SIZE] && filter[hash2 % FILTER_SIZE];
}