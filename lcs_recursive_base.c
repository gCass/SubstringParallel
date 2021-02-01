// https://www.geeksforgeeks.org/longest-common-subsequence-dp-4/

/* A Naive recursive implementation of LCS problem */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int max(int a, int b); 

char * readFile(FILE *fin, int *n) {
	int BLOCK_READ_SIZE = 200;
	char* txt, tmp;
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

/* Returns length of LCS for X[0..m-1], Y[0..n-1] */
int lcs( char *X, char *Y, int m, int n ) 
{ 
if (m == 0 || n == 0) 
	return 0; 
if (X[m-1] == Y[n-1]) 
	return 1 + lcs(X, Y, m-1, n-1); 
else
	return max(lcs(X, Y, m, n-1), lcs(X, Y, m-1, n)); 
} 

/* Utility function to get max of 2 integers */
int max(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

/* Driver program to test above function */
int main () {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    
    double t1 = omp_get_wtime();
    
    char path1[] = "dataset/stringa_lcs_1.txt";
	char path2[] = "dataset/stringa_lcs_2.txt";	
	FILE *fin;

	if((fin = fopen(path1, "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}	
	a = readFile(fin, &N);
    fclose(fin);
    
    if((fin = fopen(path2, "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}	
	b = readFile(fin, &N);
    fclose(fin);
	double t2 = omp_get_wtime();
	printf("T lettura:%f\n",t2-t1);

	//Assumo che b sia la piu' corta delle due stringhe (m<=n)
	//a = "PROVACAZZO";
	//b = "UUAU";
    
    //int n = strlen(a);
    //int m = strlen(b);
    int n = 100;
    int m = 100;
    
    printf("n=%d\n",n);
    printf("m=%d\n",m);
    
    //printf("Ciao");
    t1 = omp_get_wtime();
    int t = lcs(a, b, n, m);
    t2 = omp_get_wtime();
    printf("T function LCS:%f\n",t2-t1);
    
    printf("Stampe\n");
    
    //Stampo le stringhe di input
//	printf("%s\n",a);	
//	printf("%s\n",b);
	printf("++++++++++++++++++\n");
	//Stampo il risultato
	printf("t = %d\n", t);
//	printf("s = ");
//	printf(s);
	
	//printf("%.*s\n", t, s); // tsitest
    return 0;
}

