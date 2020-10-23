#include <stdio.h>
#include <string.h>
#include <immintrin.h>

	typedef struct {
	   unsigned int szelesseg;  //A kép szélessége (pixelben).
	   unsigned int magassag;   //A kép magassága (pixelben).
	   unsigned short szinmelyseg;  //24 bites RGB színkódok (TrueColor, maximálisan 16777216 szín).
	   unsigned int meret;  //A kép mérete
	} BITMAPINFO;

	int main()
	{
	    FILE *kep1;
	    FILE *kep2;
	    char nev1[100], nev2[100];
	    BITMAPINFO bmp1;
	    BITMAPINFO bmp2;
        long int i,j;

	    //1. kép beolvasása hibakezelessel
	    printf("Adja meg a kep nevet (a kiterjesztest is) :");
	    scanf("%s",nev1);
	    kep1=fopen(nev1,"rb");

	    while(kep1==NULL)
	    {
	        printf("Nincs ilyen nevu kep, adja meg ujra :");
	        scanf("%s",nev1);
	        kep1=fopen(nev1,"rb");
	    }
	    //2. kép beolvasása hibakezelessel
        printf("Adja meg a második kep nevet (a kiterjesztest is) :");
	    scanf("%s",nev2);
	    kep2=fopen(nev2,"rb+");

	    while(kep2==NULL)
	    {
	        printf("Nincs ilyen nevu kep, adja meg ujra :");
	        scanf("%s",nev2);
	        kep2=fopen(nev2,"rb+");
	    }

        printf("\n INFORMACIOK A KEP1-ROL: \n");
	    fseek(kep1,2,SEEK_SET);      //Az informacios fejlecben a 2. poziciotol talalhato 4byte tartalmazza a kep meretet.
	    fread(&bmp1.meret,4,1,kep1);
	    printf("A kep merete: %d\n",bmp1.meret);

	    fseek(kep1,18,SEEK_SET);     //Az informacios fejlecben a 18. poziciotol talalhato 4byte tartalmazza a kep szelesseget.
	    fread(&bmp1.szelesseg,4,1,kep1);
	    fseek(kep1,22,SEEK_SET);     //Az informacios fejlecben a 22. poziciotol talalhato 4byte tartalmazza a kep magassagat.
	    fread(&bmp1.magassag,4,1,kep1);
	    printf("A kep szelessge: %d pixel\tA kep magassaga: %d pixel\n",bmp1.szelesseg,bmp1.magassag);

	    fseek(kep1,28,SEEK_SET);    //Az informacios fejlecben a 28. poziciotol talalhato 2byte tartalmazza a kep szinmelyseget.
	    fread(&bmp1.szinmelyseg,2,1,kep1);
	    printf("A kep szinmelysege: %d bit\n",bmp1.szinmelyseg);

	    printf("\n INFORMACIOK A KEP2-ROL: \n");
	    fseek(kep2,2,SEEK_SET);      //Az informacios fejlecben a 2. poziciotol talalhato 4byte tartalmazza a kep meretet.
	    fread(&bmp2.meret,4,1,kep2);
	    printf("A kep merete: %d\n",bmp2.meret);

	    fseek(kep2,18,SEEK_SET);     //Az informacios fejlecben a 18. poziciotol talalhato 4byte tartalmazza a kep szelesseget.
	    fread(&bmp2.szelesseg,4,1,kep2);
	    fseek(kep2,22,SEEK_SET);     //Az informacios fejlecben a 22. poziciotol talalhato 4byte tartalmazza a kep magassagat.
	    fread(&bmp2.magassag,4,1,kep2);
	    printf("A kep szelessge: %d pixel\tA kep magassaga: %d pixel\n",bmp2.szelesseg,bmp2.magassag);

	    fseek(kep2,28,SEEK_SET);    //Az informacios fejlecben a 28. poziciotol talalhato 2byte tartalmazza a kep szinmelyseget.
	    fread(&bmp2.szinmelyseg,2,1,kep2);
	    printf("A kep szinmelysege: %d bit\n",bmp2.szinmelyseg);

        fseek(kep1,0,SEEK_SET); //Mivel átmásoljuk az eredménybe a kép információs header-részét ezért 0-ra állítjuk a kurzort.
        fseek(kep2,0,SEEK_SET);

        bmp1.meret = bmp1.meret-122;

        //kimenet nevenek megadasa
	    FILE *eredmeny1 = fopen("X3PQWB_bodnar_mark_armin1.bmp", "wb");
	    FILE *eredmeny2 = fopen("X3PQWB_bodnar_mark_armin2.bmp", "wb");

        //Az informacios fejlec atmasolasa a bmp filebol
        for(i = 0; i < 122; i++){
            int inforesz = fgetc(kep1);
            fputc(inforesz, eredmeny1);
            fputc(inforesz, eredmeny2);
        }

        fseek(kep1, 122, SEEK_SET);  //"...a 122. (0x7A) byte-tol kezdve vannak tarolva a pixeladatok..."
        fseek(kep2, 122, SEEK_SET);
        int *kepecske1 = (int*) malloc(bmp1.meret * sizeof(int));
        int *kepecske2 = (int*) malloc(bmp1.meret * sizeof(int));

        for(j = 0; j < bmp1.meret; j++){
            kepecske1[j] = (int)fgetc(kep1);
            kepecske2[j] = (int)fgetc(kep2);
        }

        static int ered1[5992704];   //itt sajnos nem lehet hasznalni a bmp.meretet így kenytelen voltam megadni a konkret erteket.
        static int ered2[5992704];

        //Atlagolja a ket kepet egymassal

        __m256i mm_oszto = _mm256_set1_epi32(32768 / 2);

        for(j = 0; j < bmp1.meret; j += 8){
            __m256i mm_r_1 = _mm256_loadu_si256((__m256i *)&(kepecske1[j]));
            __m256i mm_r_2 = _mm256_loadu_si256((__m256i *)&(kepecske2[j]));
            __m256i mm_r = _mm256_add_epi32(mm_r_1, mm_r_2);
            __m256i mm_ered = _mm256_mulhrs_epi16(mm_r, mm_oszto);

            _mm256_storeu_si256((__m256i *)&(ered1[j]), mm_ered);
        }
        //Mossa ossze a ket kepet ugy, hogy az elso kep 80%-ban, a masodik kep 20%-ban legyen a kimeneti kepen.

        __m256i mm_szaz = _mm256_set1_epi32(32768 / 100);
        __m256i mm_husz = _mm256_set1_epi32(20);
        __m256i mm_nyolcvan = _mm256_set1_epi32(80);

        for(j = 0; j < bmp1.meret; j += 8){
            __m256i mm_1 = _mm256_loadu_si256((__m256i *)&(kepecske1[j]));
            __m256i mm_2 = _mm256_loadu_si256((__m256i *)&(kepecske2[j]));
            __m256i mm_1_o = _mm256_mullo_epi32(mm_1, mm_nyolcvan);
            __m256i mm_2_o = _mm256_mullo_epi32(mm_2, mm_husz);
            __m256i mm_r = _mm256_add_epi32(mm_1_o, mm_2_o);
            __m256i mm_ered = _mm256_mulhrs_epi16(mm_r, mm_szaz);
            _mm256_storeu_si256((__m256i *)&(ered2[j]), mm_ered);
        }

        //az eredmeny mentese
        for(j = 0; j < bmp1.meret; j++){
            fputc(ered1[j], eredmeny1);
            fputc(ered2[j], eredmeny2);
        }

        free(kepecske1);
        free(kepecske2);

        fclose(kep1);
        fclose(kep2);
        fclose(eredmeny1);
        fclose(eredmeny2);

        return 0;
}

