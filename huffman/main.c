#include "huffman.h"
#include "huffman_memory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int test1() {
    // 테스트 데이터
    const char* test_string = "Hello, Huffman! This is a test string for compression.";
    size_t input_size = strlen(test_string);

    printf("=== 허프만 압축 테스트 ===\n");
    printf("원본 데이터: %s\n", test_string);
    printf("원본 크기: %zu 바이트\n\n", input_size);

    huffman_compressed_t* compressed;
    uint8_t* decompressed;
    size_t output_size;

    // 압축
    compressed = huffman_compress((const uint8_t*)test_string, input_size);
    if (!compressed) {
        printf("압축 실패!\n");
        return -1;
    }

    printf("압축 후 크기: %zu 바이트\n", compressed->size);
    printf("압축률: %.2f%%\n", (1.0 - (double)compressed->size / input_size) * 100);
    printf("트리 크기: %u 바이트\n\n", compressed->tree_size);

    // 해제
    output_size = 0;
    decompressed = huffman_decompress(compressed, &output_size);
    if (!decompressed) {
        printf("압축 해제 실패!\n");
        huffman_free_compressed(compressed);
        return -1;
    }

    printf("압축 해제 크기: %zu 바이트\n", output_size);
    printf("압축 해제 데이터: %.*s\n\n", (int)output_size, decompressed);

    // 검증
    if (output_size == input_size && memcmp(test_string, decompressed, input_size) == 0) {
        printf(" 압축/해제 성공! 데이터가 일치합니다.\n");
    }
    else {
        printf(" 압축/해제 실패! 데이터가 일치하지 않습니다.\n");
    }

    // 메모리 해제
    huffman_free_compressed(compressed);
    huffman_free_decompressed(decompressed);

    return 0;
}

int test2() {
    // 바이트 배열 테스트
    printf("\n=== 바이트 배열 테스트 ===\n");
    uint8_t byte_array[] = { 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03,
                            0x04, 0x05, 0x04, 0x05, 0x01, 0x01, 0x01 };
    size_t array_size = sizeof(byte_array);

    printf("원본 바이트 배열 크기: %zu 바이트\n", array_size);

    huffman_compressed_t* compressed;
    uint8_t* decompressed;
    size_t output_size;

    compressed = huffman_compress(byte_array, array_size);
    if (compressed) {
        printf("압축 후 크기: %zu 바이트\n", compressed->size);
        printf("압축률: %.2f%%\n", (1.0 - (double)compressed->size / array_size) * 100);

        decompressed = huffman_decompress(compressed, &output_size);
        if (decompressed && output_size == array_size &&
            memcmp(byte_array, decompressed, array_size) == 0) {
            printf(" 바이트 배열 압축/해제 성공!\n");
        }
        else {
            printf(" 바이트 배열 압축/해제 실패!\n");
        }

        huffman_free_compressed(compressed);
        huffman_free_decompressed(decompressed);
    }

    return 0;
}

#define TEST3_ARRAY_SIZE 256
#define TEST3_RANDOM_FACTOR 256

int test3() {
    printf("\n=== 랜덤 데이터 테스트 ===\n");

    const size_t array_size = TEST3_ARRAY_SIZE;
    uint8_t random_array[TEST3_ARRAY_SIZE];

    // 랜덤 데이터 생성
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < array_size; i++) {
        random_array[i] = rand() % TEST3_RANDOM_FACTOR;
    }

    printf("원본 랜덤 배열 크기: %zu 바이트\n", array_size);

    // 첫 16바이트 출력 (미리보기)
    printf("첫 16바이트: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", random_array[i]);
    }
    printf("\n");

    huffman_compressed_t* compressed;
    uint8_t* decompressed;
    size_t output_size;

    // 압축
    compressed = huffman_compress(random_array, array_size);
    if (!compressed) {
        printf("압축 실패!\n");
        return -1;
    }

    printf("압축 후 크기: %zu 바이트\n", compressed->size);
    printf("압축률: %.2f%%\n", (1.0 - (double)compressed->size / array_size) * 100);
    printf("트리 크기: %u 바이트\n", compressed->tree_size);

    // 압축 해제
    output_size = 0;
    decompressed = huffman_decompress(compressed, &output_size);
    if (!decompressed) {
        printf("압축 해제 실패!\n");
        huffman_free_compressed(compressed);
        return -1;
    }

    printf("압축 해제 크기: %zu 바이트\n", output_size);

    // 검증
    if (output_size == array_size && memcmp(random_array, decompressed, array_size) == 0) {
        printf(" 랜덤 데이터 압축/해제 성공! 데이터가 일치합니다.\n");

        // 해제된 첫 16바이트 확인
        printf("해제된 첫 16바이트: ");
        for (int i = 0; i < 16; i++) {
            printf("%02X ", decompressed[i]);
        }
        printf("\n");
    }
    else {
        printf(" 랜덤 데이터 압축/해제 실패! 데이터가 일치하지 않습니다.\n");
        printf("원본 크기: %zu, 해제 크기: %zu\n", array_size, output_size);
    }

    // 메모리 해제
    huffman_free_compressed(compressed);
    huffman_free_decompressed(decompressed);

    return 0;
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
