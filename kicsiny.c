#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <immintrin.h>

#define INPUT_FILE_SIZE 5992826
#define IMG_DATA_OFFSET 122

#define HEADER_FILE_SIZE_OFFSET 2
#define OUTPUT_FILE_SIZE 1498298

#define HEADER_IMG_WIDTH_OFFSET 18
#define OUTPUT_IMG_WIDTH 816

#define HEADER_IMG_HEIGHT_OFFSET 22
#define OUTPUT_IMG_HEIGHT 612

#define HEADER_IMG_SIZE_OFFSET 34
#define OUTPUT_IMG_SIZE 1498176

#define IMG_PROCESSING_UNIT_BYTES 18
#define BYTES_PER_LINE 2448

int main(int argc, char const *argv[])
{
    int fd_in, fd_out, i, j, k;
    int output_file_size = OUTPUT_FILE_SIZE;
    int output_img_width = OUTPUT_IMG_WIDTH;
    int output_img_height = OUTPUT_IMG_HEIGHT;
    int output_img_size = OUTPUT_IMG_SIZE;
    int input_img_height = OUTPUT_IMG_HEIGHT * 2;

    uint8_t *buffer_in, *buffer_out;
    uint8_t shuffle_mask[] = {
        0, 1, 2,
        6, 7, 8,
        12, 13, 14,
        18, 19, 20,
        24, 25, 26};
    __m256i mm_a, mm_b, mm_c, mm_mask;

    fd_in = open("mario.bmp", O_RDONLY);

    buffer_in = malloc(INPUT_FILE_SIZE);
    buffer_out = malloc(INPUT_FILE_SIZE);

    read(fd_in, buffer_in, INPUT_FILE_SIZE);
    memcpy(buffer_out, buffer_in, IMG_DATA_OFFSET - 1);

    memcpy(buffer_out + HEADER_FILE_SIZE_OFFSET, (int *)&output_file_size, 4);
    memcpy(buffer_out + HEADER_IMG_WIDTH_OFFSET, (int *)&output_img_width, 4);
    memcpy(buffer_out + HEADER_IMG_HEIGHT_OFFSET, (int *)&output_img_height, 4);
    memcpy(buffer_out + HEADER_IMG_SIZE_OFFSET, (int *)&output_img_size, 4);

    // half width
    for (i = IMG_DATA_OFFSET, j = 0; i < INPUT_FILE_SIZE; i += IMG_PROCESSING_UNIT_BYTES, j += 9)
    {

        mm_a = _mm256_loadu_si256((__m256i *)(buffer_in + i));
        mm_mask = _mm256_loadu_si256((__m256i *)&shuffle_mask);
        mm_b = _mm256_shuffle_epi8(mm_a, mm_mask);
        _mm256_storeu_si256((__m256i *)(buffer_out + IMG_DATA_OFFSET + j), mm_b);
    }

    // half height
    for (i = 0, k = 0; i < input_img_height; i += 2)
    {
        for (j = 0; j < BYTES_PER_LINE; j += 24, k += 24)
        {
            mm_a = _mm256_loadu_si256((__m256i *)(buffer_out + IMG_DATA_OFFSET + i * BYTES_PER_LINE + j));
            mm_b = _mm256_loadu_si256((__m256i *)(buffer_out + IMG_DATA_OFFSET + (i+1) * BYTES_PER_LINE + j));
            mm_c = _mm256_avg_epu8(mm_a, mm_b);
            _mm256_storeu_si256((__m256i *)(buffer_out + IMG_DATA_OFFSET + k), mm_c);
        }
    }

    fd_out = open("X3PQWB_Bodnár_Márk_Ármin.bmp", O_CREAT | O_WRONLY);
    write(fd_out, buffer_out, OUTPUT_FILE_SIZE);

    close(fd_in);
    close(fd_out);

    return 0;
}