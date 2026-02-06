#include "huffman.h"
#include "huffman_memory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int test(const uint8_t* input, size_t input_size) {

    printf("원본 크기: %zu 바이트\n", input_size);

    huffman_compressed_t* compressed;
    uint8_t* decompressed;
    size_t output_size;

    // 압축
    compressed = huffman_compress((const uint8_t*)input, input_size);
    if (!compressed) {
        printf("압축 실패!\n");
        return -1;
    }

    printf("압축 후 크기: %zu 바이트\n", compressed->size);
    printf("압축률: %.2f%%\n", (1.0 - (double)compressed->size / input_size) * 100);
    printf("트리 크기: %u 바이트\n", compressed->tree_size);

    // 해제
    output_size = 0;
    decompressed = huffman_decompress(compressed, &output_size);
    if (!decompressed) {
        printf("압축 해제 실패!\n");
        huffman_free_compressed(compressed);
        return -1;
    }

    printf("압축 해제 크기: %zu 바이트\n", output_size);

    // 검증
    if (output_size == input_size && memcmp(input, decompressed, input_size) == 0) {
        printf("압축/해제 성공! 데이터가 일치합니다.\n");
    }
    else {
        printf("압축/해제 실패! 데이터가 일치하지 않습니다.\n");
    }

    huffman_free_compressed(compressed);
    huffman_free_decompressed(decompressed);

    return 0;
}

void test1() {
    printf("=== 허프만 압축 테스트 ===\n");

    const char* input = "Hello, Huffman! This is a test string for compression.";
    size_t input_size = strlen(input);

	test((const uint8_t*)input, input_size);
}

void test2() {
    printf("=== 바이트 배열 테스트 ===\n");

    uint8_t input[] = { 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x04, 0x05, 0x04, 0x05, 0x01, 0x01, 0x01 };
    size_t input_size = sizeof(input);

	test((const uint8_t*)input, input_size);
}

void test3() {
    printf("=== 랜덤 데이터 테스트 ===\n");

    #define TEST3_ARRAY_SIZE 128
    #define TEST3_RANDOM_FACTOR 32

    const size_t input_size = TEST3_ARRAY_SIZE;
    uint8_t input[TEST3_ARRAY_SIZE];

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < input_size; i++) {
        input[i] = rand() % TEST3_RANDOM_FACTOR;
    }
    
    test((const uint8_t*)input, input_size);
}

int main() {
    test1();
    huffman_crt_dump();

    huffman_crt_reset_dump();

    test2();
    huffman_crt_dump();

    huffman_crt_reset_dump();

    test3();
    huffman_crt_dump();

    return 0;
}

/*
=== 허프만 압축 테스트 ===
원본 크기: 54 바이트
압축 후 크기: 57 바이트
압축률: -5.56%
트리 크기: 28 바이트
압축 해제 크기: 54 바이트
압축/해제 성공! 데이터가 일치합니다.
# Huffman CRT Memory Dump:
  Allocation count: 88
  Total Allocated:  2787 bytes
  Total Freed:      2787 bytes

=== 바이트 배열 테스트 ===
원본 크기: 16 바이트
압축 후 크기: 12 바이트
압축률: 25.00%
트리 크기: 7 바이트
압축 해제 크기: 16 바이트
압축/해제 성공! 데이터가 일치합니다.
# Huffman CRT Memory Dump:
  Allocation count: 22
  Total Allocated:  1464 bytes
  Total Freed:      1464 bytes

=== 랜덤 데이터 테스트 ===
원본 크기: 128 바이트
압축 후 크기: 119 바이트
압축률: 7.03%
트리 크기: 40 바이트
압축 해제 크기: 128 바이트
압축/해제 성공! 데이터가 일치합니다.
# Huffman CRT Memory Dump:
  Allocation count: 131
  Total Allocated:  3859 bytes
  Total Freed:      3859 bytes
*/
