//Credits: http://www.rosettacode.org/wiki/Longest_common_subsequence#C
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
	//Initialize the string
	strcpy(txt,"");		
	
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
    
	//Assign to variable c the pointers to the different rows of the matrix 
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= m; j++) {        
        	
            if (a[i - 1] == b[j - 1]) {
                c[i][j] = c[i - 1][j - 1] + 1;
            }
            else {
                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            }
        }
    }
    
    
    //t contains length of lcs string
    t = c[n][m];
    //Allocate space for the lcs string
    *s = malloc(t);
	//Rebuild of the lcs string
    for (i = n, j = m, k = t - 1; k >= 0;) {
        if (a[i - 1] == b[j - 1])
            (*s)[k] = a[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            j--;
        else
            i--;
    }
    
    free(c);
    free(z);
    return t;
}

int main (int argc, char *argv[]) {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    double t1,t2;
    
    
    //The parameters are the 2 file paths
    if(argc < 3) {
    	printf("Inserire come parametri del programma i nomi dei due file\n");
    	return 0;
	}
	
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
    
    int n = strlen(a);
    int m = strlen(b);
    
    t1 = omp_get_wtime();
    int t = lcs(a, n, b, m, &s);
    t2 = omp_get_wtime();
  	printf("T_lcs %f\n",t2-t1);


	//Print length of lcs string
	//printf("%d\n",t);
	
	//Print the lcs string
	//printf("%.*s\n", t, s); 
    return 0;
}
