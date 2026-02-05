#ifndef HUFFMAN_MEMORY_H
#define HUFFMAN_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

void* huffman_crt_calloc(size_t count, size_t size);
void* huffman_crt_malloc(size_t size);
void huffman_crt_free(void* ptr);

void huffman_crt_memset(void* ptr, int value, size_t num);
void huffman_crt_memcpy(void* dest, const void* src, size_t num);

void huffman_crt_reset_dump(void);
void huffman_crt_dump(void);

#if defined(__cplusplus)
}
#endif

#endif // HUFFMAN_MEMORY_H

