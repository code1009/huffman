# 허프만 압축 모듈 (C언어)

허프만 알고리즘을 사용하여 바이트 배열을 압축하고 해제하는 C 라이브러리입니다.

## 주요 기능

- 🗜️ 바이트 배열 압축 (허프만 코딩)
- 📂 압축 해제
- 🌳 허프만 트리 직렬화/역직렬화
- 💾 메모리 효율적 처리
- ✅ 모든 바이트 값 지원 (0x00 ~ 0xFF)

## 파일 구조

```
huffman.h       - 헤더 파일 (API 정의)
huffman.c       - 구현 파일
example.c       - 사용 예제
README.md       - 문서
```

## 컴파일

```bash
# 예제 프로그램 컴파일
gcc -o huffman_example huffman.c example.c -Wall -O2

# 실행
./huffman_example
```

## 사용 방법

### 1. 압축

```c
#include "huffman.h"

// 입력 데이터
uint8_t data[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
size_t size = sizeof(data);

// 압축
huffman_compressed_t *compressed = huffman_compress(data, size);
if (compressed) {
    printf("압축 전: %zu 바이트\n", size);
    printf("압축 후: %zu 바이트\n", compressed->size);
    
    // 사용 후 메모리 해제
    huffman_free_compressed(compressed);
}
```

### 2. 압축 해제

```c
// 압축된 데이터 해제
size_t output_size = 0;
uint8_t *decompressed = huffman_decompress(compressed, &output_size);

if (decompressed) {
    printf("해제된 크기: %zu 바이트\n", output_size);
    
    // 사용 후 메모리 해제
    huffman_free_decompressed(decompressed);
}
```

### 3. 완전한 예제

```c
#include "huffman.h"
#include <string.h>
#include <stdio.h>

int main() {
    const char *text = "Hello, World!";
    size_t input_size = strlen(text);
    
    // 압축
    huffman_compressed_t *compressed = huffman_compress(
        (const uint8_t*)text, input_size
    );
    
    if (!compressed) {
        printf("압축 실패\n");
        return 1;
    }
    
    // 압축 해제
    size_t output_size = 0;
    uint8_t *decompressed = huffman_decompress(compressed, &output_size);
    
    if (!decompressed) {
        printf("압축 해제 실패\n");
        huffman_free_compressed(compressed);
        return 1;
    }
    
    // 검증
    if (memcmp(text, decompressed, input_size) == 0) {
        printf("성공!\n");
        printf("압축률: %.2f%%\n", 
            (1.0 - (double)compressed->size / input_size) * 100);
    }
    
    // 메모리 해제
    huffman_free_compressed(compressed);
    huffman_free_decompressed(decompressed);
    
    return 0;
}
```

## API 레퍼런스

### huffman_compress()

```c
huffman_compressed_t* huffman_compress(const uint8_t *input, size_t input_size);
```

바이트 배열을 압축합니다.

**매개변수:**
- `input`: 압축할 데이터
- `input_size`: 입력 데이터 크기

**반환값:** 압축된 데이터 구조체 (실패 시 NULL)

### huffman_decompress()

```c
uint8_t* huffman_decompress(const huffman_compressed_t *compressed, size_t *output_size);
```

압축된 데이터를 해제합니다.

**매개변수:**
- `compressed`: 압축된 데이터
- `output_size`: 출력 크기를 저장할 포인터

**반환값:** 해제된 데이터 (실패 시 NULL)

### huffman_free_compressed()

```c
void huffman_free_compressed(huffman_compressed_t *compressed);
```

압축된 데이터 메모리를 해제합니다.

### huffman_free_decompressed()

```c
void huffman_free_decompressed(uint8_t *data);
```

해제된 데이터 메모리를 해제합니다.

## 알고리즘 설명

### 1. 압축 프로세스

1. **빈도수 계산**: 입력 데이터의 각 바이트 빈도 계산
2. **허프만 트리 구축**: 최소 힙을 사용하여 트리 생성
3. **코드 생성**: 트리를 순회하여 각 바이트에 대한 가변 길이 코드 생성
4. **트리 직렬화**: 압축 해제를 위해 트리 구조 저장
5. **데이터 인코딩**: 생성된 코드로 데이터 압축

### 2. 압축 해제 프로세스

1. **트리 역직렬화**: 저장된 트리 구조 복원
2. **데이터 디코딩**: 트리를 사용하여 비트 스트림 디코딩

## 시간/공간 복잡도

- **압축 시간**: O(n log n) - n은 입력 크기
- **압축 해제 시간**: O(n)
- **공간 복잡도**: O(n + 256) - 고정 크기 알파벳

## 특징 및 제한사항

### 특징
- ✅ 모든 바이트 값 지원 (바이너리 데이터 가능)
- ✅ 단일 바이트 값 처리 (특수 케이스)
- ✅ 메모리 안전 (NULL 체크 및 경계 검사)
- ✅ 트리 포함 완전한 압축 포맷

### 제한사항
- 작은 데이터의 경우 트리 오버헤드로 인해 압축률이 낮을 수 있음
- 균등 분포 데이터는 압축 효과가 적음
- 메모리 내 처리 (스트리밍 지원 안 함)

## 라이선스

MIT License - 자유롭게 사용하세요.