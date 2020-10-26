#include <stdio.h>
#include <immintrin.h>

int main(){
	
    int i=0;
	int result = 0;
    float *buffer;
     __m256 lower_then = _mm256_set1_ps(0); //0tól kisebb akkor negativ
	 
    FILE* floats_c = fopen("/home/soos/feladat/floats.2gb.bin","rb");
    if (floats_c  == NULL){
        printf("Nem talalom a fajlt\n");
        return -1;
    }
    fseek(floats_c , 0 ,SEEK_END);
    long len = ftell(floats_c );
    printf("Méret : %ld \n", len);
    rewind(floats_c );
	
    buffer = aligned_alloc(32, len);
    fread(buffer,4,len/4,floats_c );
    for(i;i<len/4;i+=8){
		float = _256_load_ps(&(buffer[i]));
		greater = _mm256-cmp-ps(floats, lower_then, _CMP_GT_OQ);
		result +=_mm_popcnt_u32(_mm256_movemask_ps(greater));
       //result+=_mm_popcnt_u32(_mm256_movemask_ps(_mm256_cmp_ps(_mm256_load_ps(&(buffer[i])),lower_then,_CMP_GT_OQ)));
    }
	
    printf("%d elem van \n", result);
    free(buffer);
    fclose(floats_c );
    return 0;
}

