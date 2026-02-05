#include "huffman.h"
#include "huffman_memory.h"
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

#define MAX_TREE_NODES 512
#define BYTE_SIZE 256

// 허프만 트리 노드
typedef struct huffman_node {
    uint8_t byte;
    uint32_t frequency;
    struct huffman_node* left;
    struct huffman_node* right;
} huffman_node_t;

// 우선순위 큐 (최소 힙)
typedef struct {
    huffman_node_t** nodes;
    size_t size;
    size_t capacity;
} priority_queue_t;

// 비트 버퍼 (압축 시 사용)
typedef struct {
    uint8_t* data;
    size_t byte_pos;
    uint8_t bit_pos;
    size_t capacity;
} bit_buffer_t;

// 허프만 코드 테이블
typedef struct {
    uint32_t code;
    uint8_t length;
} huffman_code_t;

// ============== 우선순위 큐 함수 ==============

static priority_queue_t* pq_create(size_t capacity) {
    priority_queue_t* pq = (priority_queue_t*)huffman_crt_malloc(sizeof(priority_queue_t));
    if (!pq) return NULL;

    pq->nodes = (huffman_node_t**)huffman_crt_malloc(sizeof(huffman_node_t*) * capacity);
    if (!pq->nodes) {
        huffman_crt_free(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

static void pq_swap(huffman_node_t** a, huffman_node_t** b) {
    huffman_node_t* temp = *a;
    *a = *b;
    *b = temp;
}

static void pq_heapify_up(priority_queue_t* pq, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (pq->nodes[parent]->frequency <= pq->nodes[idx]->frequency)
            break;
        pq_swap(&pq->nodes[parent], &pq->nodes[idx]);
        idx = parent;
    }
}

static void pq_heapify_down(priority_queue_t* pq, size_t idx) {
    while (1) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;

        if (left < pq->size && pq->nodes[left]->frequency < pq->nodes[smallest]->frequency)
            smallest = left;
        if (right < pq->size && pq->nodes[right]->frequency < pq->nodes[smallest]->frequency)
            smallest = right;

        if (smallest == idx)
            break;

        pq_swap(&pq->nodes[idx], &pq->nodes[smallest]);
        idx = smallest;
    }
}

static void pq_push(priority_queue_t* pq, huffman_node_t* node) {
    if (pq->size >= pq->capacity) return;

    pq->nodes[pq->size] = node;
    pq_heapify_up(pq, pq->size);
    pq->size++;
}

static huffman_node_t* pq_pop(priority_queue_t* pq) {
    if (pq->size == 0) return NULL;

    huffman_node_t* min = pq->nodes[0];
    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;
    pq_heapify_down(pq, 0);

    return min;
}

static void pq_free(priority_queue_t* pq) {
    if (pq) {
        huffman_crt_free(pq->nodes);
        huffman_crt_free(pq);
    }
}

// ============== 허프만 노드 함수 ==============

static huffman_node_t* create_node(uint8_t byte, uint32_t frequency) {
    huffman_node_t* node = (huffman_node_t*)huffman_crt_malloc(sizeof(huffman_node_t));
    if (!node) return NULL;

    node->byte = byte;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static void free_tree(huffman_node_t* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    huffman_crt_free(root);
}

// ============== 비트 버퍼 함수 ==============

static bit_buffer_t* create_bit_buffer(size_t capacity) {
    bit_buffer_t* bb = (bit_buffer_t*)huffman_crt_malloc(sizeof(bit_buffer_t));
    if (!bb) return NULL;

    bb->data = (uint8_t*)huffman_crt_calloc(capacity, 1);
    if (!bb->data) {
        huffman_crt_free(bb);
        return NULL;
    }

    bb->byte_pos = 0;
    bb->bit_pos = 0;
    bb->capacity = capacity;

    return bb;
}

static void write_bit(bit_buffer_t* bb, uint8_t bit) {
    if (bb->byte_pos >= bb->capacity) return;

    if (bit)
        bb->data[bb->byte_pos] |= (1 << (7 - bb->bit_pos));

    bb->bit_pos++;
    if (bb->bit_pos == 8) {
        bb->bit_pos = 0;
        bb->byte_pos++;
    }
}

static void write_bits(bit_buffer_t* bb, uint32_t code, uint8_t length) {
    for (int i = length - 1; i >= 0; i--) {
        write_bit(bb, (code >> i) & 1);
    }
}

static uint8_t read_bit(const uint8_t* data, size_t* byte_pos, uint8_t* bit_pos) {
    uint8_t bit = (data[*byte_pos] >> (7 - *bit_pos)) & 1;
    (*bit_pos)++;
    if (*bit_pos == 8) {
        *bit_pos = 0;
        (*byte_pos)++;
    }
    return bit;
}

// ============== 허프만 트리 구축 ==============

static huffman_node_t* build_huffman_tree(const uint8_t* input, size_t input_size) {
    // 빈도수 계산
    uint32_t frequency[BYTE_SIZE] = { 0 };
    for (size_t i = 0; i < input_size; i++) {
        frequency[input[i]]++;
    }

    // 우선순위 큐 생성
    priority_queue_t* pq = pq_create(BYTE_SIZE);
    if (!pq) return NULL;

    // 빈도수가 0이 아닌 바이트에 대해 리프 노드 생성
    for (int i = 0; i < BYTE_SIZE; i++) {
        if (frequency[i] > 0) {
            huffman_node_t* node = create_node((uint8_t)i, frequency[i]);
            if (node)
                pq_push(pq, node);
        }
    }

    // 특수 케이스: 하나의 유니크한 바이트만 있는 경우
    if (pq->size == 1) {
        huffman_node_t* single = pq_pop(pq);
        huffman_node_t* root = create_node(0, single->frequency);
        root->left = single;
        pq_free(pq);
        return root;
    }

    // 허프만 트리 구축
    while (pq->size > 1) {
        huffman_node_t* left = pq_pop(pq);
        huffman_node_t* right = pq_pop(pq);

        huffman_node_t* parent = create_node(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        pq_push(pq, parent);
    }

    huffman_node_t* root = pq_pop(pq);
    pq_free(pq);

    return root;
}

// ============== 코드 생성 ==============

static void generate_codes_recursive(huffman_node_t* node, uint32_t code, uint8_t depth, huffman_code_t* codes) {
    if (!node) return;

    // 리프 노드
    if (!node->left && !node->right) {
        codes[node->byte].code = code;
        codes[node->byte].length = depth;
        return;
    }

    if (node->left)
        generate_codes_recursive(node->left, (code << 1) | 0, depth + 1, codes);
    if (node->right)
        generate_codes_recursive(node->right, (code << 1) | 1, depth + 1, codes);
}

static void generate_codes(huffman_node_t* root, huffman_code_t* codes) {
    huffman_crt_memset(codes, 0, sizeof(huffman_code_t) * BYTE_SIZE);

    // 특수 케이스: 루트만 있는 경우
    if (root && !root->left && !root->right) {
        codes[root->byte].code = 0;
        codes[root->byte].length = 1;
        return;
    }

    generate_codes_recursive(root, 0, 0, codes);
}

// ============== 트리 직렬화/역직렬화 ==============

static void serialize_tree_recursive(huffman_node_t* node, bit_buffer_t* bb) {
    if (!node) return;

    // 리프 노드: 1비트(1) + 8비트(바이트)
    if (!node->left && !node->right) {
        write_bit(bb, 1);
        write_bits(bb, node->byte, 8);
        return;
    }

    // 내부 노드: 1비트(0)
    write_bit(bb, 0);
    serialize_tree_recursive(node->left, bb);
    serialize_tree_recursive(node->right, bb);
}

static huffman_node_t* deserialize_tree_recursive(const uint8_t* data, size_t* byte_pos, uint8_t* bit_pos) {
    uint8_t is_leaf = read_bit(data, byte_pos, bit_pos);

    if (is_leaf) {
        uint8_t byte = 0;
        for (int i = 0; i < 8; i++) {
            byte = (byte << 1) | read_bit(data, byte_pos, bit_pos);
        }
        return create_node(byte, 0);
    }

    huffman_node_t* node = create_node(0, 0);
    node->left = deserialize_tree_recursive(data, byte_pos, bit_pos);
    node->right = deserialize_tree_recursive(data, byte_pos, bit_pos);

    return node;
}

// ============== 압축/해제 함수 ==============

huffman_compressed_t* huffman_compress(const uint8_t* input, size_t input_size) {
    if (!input || input_size == 0) return NULL;

    // 허프만 트리 구축
    huffman_node_t* root = build_huffman_tree(input, input_size);
    if (!root) return NULL;

    // 코드 생성
    huffman_code_t codes[BYTE_SIZE];
    generate_codes(root, codes);

    // 트리 직렬화
    bit_buffer_t* tree_buffer = create_bit_buffer(input_size * 2);
    if (!tree_buffer) {
        free_tree(root);
        return NULL;
    }
    serialize_tree_recursive(root, tree_buffer);
    size_t tree_bytes = tree_buffer->byte_pos + (tree_buffer->bit_pos > 0 ? 1 : 0);

    // 데이터 압축
    bit_buffer_t* data_buffer = create_bit_buffer(input_size * 2);
    if (!data_buffer) {
        huffman_crt_free(tree_buffer->data);
        huffman_crt_free(tree_buffer);
        free_tree(root);
        return NULL;
    }

    for (size_t i = 0; i < input_size; i++) {
        huffman_code_t code = codes[input[i]];
        write_bits(data_buffer, code.code, code.length);
    }
    size_t data_bytes = data_buffer->byte_pos + (data_buffer->bit_pos > 0 ? 1 : 0);

    // 결과 구조체 생성
    huffman_compressed_t* compressed = (huffman_compressed_t*)huffman_crt_malloc(sizeof(huffman_compressed_t));
    if (!compressed) {
        huffman_crt_free(tree_buffer->data);
        huffman_crt_free(tree_buffer);
        huffman_crt_free(data_buffer->data);
        huffman_crt_free(data_buffer);
        free_tree(root);
        return NULL;
    }

    // 트리 + 데이터 결합
    compressed->tree_size = (uint32_t)tree_bytes;
    compressed->size = tree_bytes + data_bytes;
    compressed->original_size = input_size;
    compressed->data = (uint8_t*)huffman_crt_malloc(compressed->size);

    if (!compressed->data) {
        huffman_crt_free(compressed);
        huffman_crt_free(tree_buffer->data);
        huffman_crt_free(tree_buffer);
        huffman_crt_free(data_buffer->data);
        huffman_crt_free(data_buffer);
        free_tree(root);
        return NULL;
    }

    huffman_crt_memcpy(compressed->data, tree_buffer->data, tree_bytes);
    huffman_crt_memcpy(compressed->data + tree_bytes, data_buffer->data, data_bytes);

    // 메모리 정리
    huffman_crt_free(tree_buffer->data);
    huffman_crt_free(tree_buffer);
    huffman_crt_free(data_buffer->data);
    huffman_crt_free(data_buffer);
    free_tree(root);

    return compressed;
}

uint8_t* huffman_decompress(const huffman_compressed_t* compressed, size_t* output_size) {
    if (!compressed || !compressed->data || compressed->size == 0) return NULL;

    // 트리 역직렬화
    size_t byte_pos = 0;
    uint8_t bit_pos = 0;
    huffman_node_t* root = deserialize_tree_recursive(compressed->data, &byte_pos, &bit_pos);
    if (!root) return NULL;

    // 출력 버퍼 할당
    uint8_t* output = (uint8_t*)huffman_crt_malloc(compressed->original_size);
    if (!output) {
        free_tree(root);
        return NULL;
    }

    // 데이터 해제
    byte_pos = compressed->tree_size;
    bit_pos = 0;

    huffman_node_t* current = root;
    size_t decoded = 0;

    while (decoded < compressed->original_size) {
        // 특수 케이스: 루트만 있는 경우
        if (!root->left && !root->right) {
            output[decoded++] = root->byte;
            continue;
        }

        if (byte_pos >= compressed->size) break;

        uint8_t bit = read_bit(compressed->data, &byte_pos, &bit_pos);

        current = bit ? current->right : current->left;

        if (!current->left && !current->right) {
            output[decoded++] = current->byte;
            current = root;
        }
    }

    free_tree(root);

    if (output_size)
        *output_size = decoded;

    return output;
}

void huffman_free_compressed(huffman_compressed_t* compressed) {
    if (compressed) {
        huffman_crt_free(compressed->data);
        huffman_crt_free(compressed);
    }
}

void huffman_free_decompressed(uint8_t* data) {
    huffman_crt_free(data);
}
