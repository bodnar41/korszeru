#include <stdio.h>
#include <immintrin.h>

void main()
{
    FILE *file = fopen("../../../../korszeru/floats.2gb.bin", "rb");
    int i;

    float *Buff = (float*)aligned_alloc(32, 32),
          *Ered = (float*)aligned_alloc(32, 32);

    __m256 result = _mm256_setzero_ps(),
           zero = _mm256_setzero_ps(),
           comp = _mm256_set1_ps(5.21);

    for (i = 0; i < 2147483648 / 4; i += 8)  // 2GB
    {
        fread(Buff, 32, 1, file);
        __m256 a = _mm256_load_ps(Buff);
        __m256 maszk = _mm256_cmp_ps(a, comp, _CMP_GT_OQ); // összehas
        a = _mm256_blendv_ps(a, zero, maszk); // nullazas a nemjo ertekeket
        result = _mm256_add_ps(result, a);
    }
    _mm256_store_ps(Ered, result); //eredmenyek elmentese

    float sum = 0;
    for (i = 0; i < 8; i++)
    {
        sum += Ered[i];
    }
    printf("A negatív elemek száma: %.2f\n", sum);

}
