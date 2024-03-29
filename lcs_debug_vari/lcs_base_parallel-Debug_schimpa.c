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
    int *z = calloc((n + 1) * (m + 1), sizeof (int));	//NB: la matrice c e' (n+1)x(m+1), non nxm!!!
    int **c = calloc((n + 1), sizeof (int *));
    
	//Assegno a c i puntatori che puntano alle varie posizioni di z (vedere disegno su paint)
	//Queste iterazioni sono tutte indipendenti
	#pragma omp parallel for
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
//    //Dynamic programming che mette nella matrice i vari risultati di lcs ecc...
//    for (i = 1; i <= n; i++) {
//        for (j = 1; j <= m; j++) {
//            if (a[i - 1] == b[j - 1]) {
//                c[i][j] = c[i - 1][j - 1] + 1;
//            }
//            else {
//                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
//            }
//        }
//    }
    
    k=0;
    int l=0;
    //Itero sui blob blu
    for (k=1; k <= n+m-1; k++){
    	//printf("K:%d\n",k);
    	//Itero sulle operazioni interne al blob
		if(k <= n){
			for (l=1; l <= k; l++){				
				//printf("l:%d\n",l);

				i = l;
				j = k-l+1;//Io voglio sempre i+j=k+1
				
				printf("i:%d\n",i);
				printf("j:%d\n",j);
				printf("\n\n");

						
    			if (a[i - 1] == b[j - 1]) {
    			  
             	  c[i][j] = c[i - 1][j - 1] + 1;
            	}
            	else {
             	   c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            	}
			}	
		}else {
			printf("Sotto\n");
			for (l=k-n+1; l <= n; l++){
    			//printf("l:%d\n",l);

    			i = l;
				j = k-l+1; //Io voglio sempre i+j=k+1
				
				printf("i:%d\n",i);
				printf("j:%d\n",j);
				printf("\n\n");
    			
    			if (a[i - 1] == b[j - 1]) {
             	  c[i][j] = c[i - 1][j - 1] + 1;
            	}
            	else {
             	   c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            	}
    			
    			
			}
		}
		 	
	}
    

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
    
    //Libero lo spazio
    free(c);
    free(z);
    return t;
}

int main () {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    
//    char path1[] = "dataset/stringa_lcs_1.txt";
//	char path2[] = "dataset/stringa_lcs_2.txt";	
//	FILE *fin;
//
//	if((fin = fopen(path1, "r"))==NULL){
//		printf("Errore nell'apertura del file!");
//		return -1;
//	}	
//	a = readFile(fin, &N);
//    fclose(fin);
//    
//    if((fin = fopen(path2, "r"))==NULL){
//		printf("Errore nell'apertura del file!");
//		return -1;
//	}	
//	b = readFile(fin, &N);
//    fclose(fin);


	a = "CAGATA";
	b = "BANANA";
    
    int n = strlen(a);
    int m = strlen(b);
    
    //printf("Ciao");
    
    int t = lcs(a, n, b, m, &s);
    
    printf("Stampe\n");
    
	printf("%s\n",a);
	//printf("\n");
	
	printf("%s\n",b);
	printf("++++++++++++++++++\n");
	
	printf("t = %d\n", t);
	printf("s = %s\n", s);
	
	//printf("%.*s\n", t, s); // tsitest
    return 0;
}
