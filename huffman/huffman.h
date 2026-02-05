#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

// 압축된 데이터 구조
typedef struct {
    uint8_t* data;          // 압축된 데이터
    size_t size;            // 압축된 데이터 크기 (바이트)
    size_t original_size;   // 원본 데이터 크기
    uint32_t tree_size;     // 직렬화된 트리 크기
} huffman_compressed_t;

/**
 * 바이트 배열을 허프만 알고리즘으로 압축
 * @param input 입력 데이터
 * @param input_size 입력 데이터 크기
 * @return 압축된 데이터 구조체 (실패 시 NULL)
 */
huffman_compressed_t* huffman_compress(const uint8_t* input, size_t input_size);

/**
 * 허프만 압축된 데이터를 해제
 * @param compressed 압축된 데이터
 * @param output_size 출력 데이터 크기를 저장할 포인터
 * @return 해제된 데이터 (실패 시 NULL)
 */
uint8_t* huffman_decompress(const huffman_compressed_t* compressed, size_t* output_size);

/**
 * 압축된 데이터 구조체 메모리 해제
 * @param compressed 해제할 구조체
 */
void huffman_free_compressed(huffman_compressed_t* compressed);

/**
 * 해제된 데이터 메모리 해제
 * @param data 해제할 데이터
 */
void huffman_free_decompressed(uint8_t* data);

#if defined(__cplusplus)
}
#endif

#endif // HUFFMAN_H