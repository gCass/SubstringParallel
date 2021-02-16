#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
 
 
#define MAX(a, b) (a > b ? a : b)
  
char * readFile(FILE *fin, int *n);

char * readFile(FILE *fin, int *n) {
	int BLOCK_READ_SIZE = 200;
	char * txt, tmp;
	int dim=8, read, i, j;
	char buf[BLOCK_READ_SIZE];
	(*n)=0;
		
	txt = malloc (dim * BLOCK_READ_SIZE * sizeof(char));
	strcpy(txt,"");		//Inizializzo la stringa
	
	if(!txt) {
		return NULL;
	}
	while(fgets(buf, sizeof(buf), fin)) {
		
		txt = strcat(txt,buf);		
		(*n)++;
		if((*n)>=dim) {
			dim *= 2;
			txt = realloc(txt, dim * BLOCK_READ_SIZE * sizeof(char));
			if(!txt){
				return NULL;
			}
		}
		
	}
	txt = realloc (txt, (*n) * BLOCK_READ_SIZE * sizeof(char));
	return txt;
}

 
int lcs (char *a, int n, char *b, int m, char **s) {
    int i, j, k, t;
    int *z = calloc((n + 1) * (m + 1), sizeof (int));
    int **c = calloc((n + 1), sizeof (int *));
    
	//Assegno a c i puntatori che puntano alle varie posizioni di z (vedere disegno su paint)
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
    //Dynamic programming che mette nella matrice i vari risultati di lcs ecc...
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= m; j++) {
        	
//			printf("i:%d\n",i);
//			printf("j:%d\n",j);
//			printf("\n\n");
        	
            if (a[i - 1] == b[j - 1]) {
                c[i][j] = c[i - 1][j - 1] + 1;
            }
            else {
                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            }
        }
    }
    
//    printf("Matrice c\n");
//	for(i=0; i<=n; i++){
//		for(j=0; j<=m; j++){
//			printf("%d ",c[i][j]);
//		}
//		printf("\n");
//	}
    
    
    /*
    //t contiene il valore finale della lunghezza della lcs
    t = c[n][m];
    //Alloco lo spazio per la lcs
    *s = malloc(t);
    //Ricostruisco la lcs
    for (i = n, j = m, k = t - 1; k >= 0;) {
        if (a[i - 1] == b[j - 1])
            (*s)[k] = a[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            j--;
        else
            i--;
    }*/
    
    t = ricostruzioneStringa(c, s, a, n, b, m);
    
    //Libero lo spazio
    free(c);
    free(z);
    return t;
}

int ricostruzioneStringa(int **c, char **s, char *a, int n, char *b, int m){
	int t, i, j, k;
	//t contiene il valore finale della lunghezza della lcs
    t = c[n][m];
    //Alloco lo spazio per la lcs
    *s = malloc(t);
    //Ricostruisco la lcs
    for (i = n, j = m, k = t - 1; k >= 0;) {
        if (a[i - 1] == b[j - 1])
            (*s)[k] = a[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            j--;
        else
            i--;
    }
    return t;
}

int main (int argc, char *argv[]) {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    double t1,t2;
    
    
    // Come parametri vogliamo i nomi dei due file e la dimensione del blocco.
    // Tra i parametri viene sempre considerato anche il nome del programma
    if(argc < 3) {
    	printf("Inserire come parametri del programma i nomi dei due file\n");
    	return 0;
	}
	
    //char path1[] = "dataset/stringa_lcs_3.txt";
	//char path2[] = "dataset/stringa_lcs_4.txt";	
	FILE *fin;
	t1 = omp_get_wtime();
	if((fin = fopen(argv[1], "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}	
	a = readFile(fin, &N);
    fclose(fin);
    
    if((fin = fopen(argv[2], "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}	
	b = readFile(fin, &N);
    fclose(fin);
	t2 = omp_get_wtime();
  	printf("T_lettura %f\n",t2-t1);

//	a = "UUUAOUUUAOUUUAOUUUAOUUUAO";
//	b = "PROVACAZZOPROVA";   

    
    int n = strlen(a+3);
    int m = strlen(b+3);
    
    t1 = omp_get_wtime();
    int t = lcs(a+3, n, b+3, m, &s);
    t2 = omp_get_wtime();
  	printf("T_lcs %f\n",t2-t1);


    
//	printf(a);
//	printf("\n");
//	
//	printf(b);
//	printf("\n");
	
	//printf("%.*s\n", t, s); // tsitest
    return 0;
}
