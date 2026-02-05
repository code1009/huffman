#include "huffman_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>

static size_t _memory_allocated_size = 0;
static size_t _memory_freed_size = 0;
std::map<void*, size_t> _memory_allocation;

extern "C"
void* huffman_crt_calloc(size_t count, size_t size)
{
	_memory_allocated_size += (count * size);

	void* ptr = calloc(count, size);
	
	_memory_allocation[ptr] = count * size;

	return ptr;
}

extern "C"
void* huffman_crt_malloc(size_t size)
{
	_memory_allocated_size += size;

	void* ptr = malloc(size);

	_memory_allocation[ptr] = size;

	return ptr;
}

extern "C"
void huffman_crt_free(void* ptr)
{
	if (ptr != nullptr) {
		_memory_freed_size += _memory_allocation[ptr];
		_memory_allocation.erase(ptr);
	}
	free(ptr);
}

extern "C"
void huffman_crt_memset(void* ptr, int value, size_t num)
{
	memset(ptr, value, num);
}

extern "C"
void huffman_crt_memcpy(void* dest, const void* src, size_t num)
{
	memcpy(dest, src, num);
}

extern "C"
void huffman_crt_reset_dump(void)
{
	_memory_allocated_size = 0;
	_memory_freed_size = 0;
}

extern "C"
void huffman_crt_dump(void)
{
	printf("Huffman CRT Memory Dump:\n");
	printf("  Total Allocated: %zu bytes\n", _memory_allocated_size);
	printf("  Total Freed:     %zu bytes\n", _memory_freed_size);
}
