// Rabin Karp algorithm
// Source: https://www.geeksforgeeks.org/rabin-karp-algorithm-for-pattern-searching/

/* Following program is a C implementation of Rabin Karp 
Algorithm given in the CLRS book */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


// d is the number of characters in the input alphabet 
#define NO_OF_CHARS 256

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


/* pat -> pattern 
	txt -> text 
	q -> A prime number 
*/
long* search(char *pat, char *txt, int q) 
{ 
	int M = strlen(pat); 
	int N = strlen(txt); 
	long i, j; 
	int p = 0; // hash value for pattern 
	int t = 0; // hash value for txt 
	int h = 1; 
	
	long* r; //r[0] contiene il numero di elementi del vettore 
	int dim=20;

	
	/* A loop to slide pat[] one by one */
	
	r = malloc(dim * sizeof(long));
	
	if(!r){
		printf("Error malloc");
		return NULL;
	}
	
	r[0] = 0;
	

	// The value of h would be "pow(NO_OF_CHARS, M-1)%q" 
	for (i = 0; i < M-1; i++) 
		h = (h*NO_OF_CHARS)%q; 

	// Calculate the hash value of pattern and first 
	// window of text 
	for (i = 0; i < M; i++) 
	{ 
		p = (NO_OF_CHARS*p + pat[i])%q; 
		t = (NO_OF_CHARS*t + txt[i])%q; 
	} 

	// Slide the pattern over text one by one 
	for (i = 0; i <= N - M; i++) 
	{ 

		// Check the hash values of current window of text 
		// and pattern. If the hash values match then only 
		// check for characters on by one 
		if ( p == t ) 
		{ 
			/* Check for characters one by one */
			for (j = 0; j < M; j++) 
			{ 
				if (txt[i+j] != pat[j]) 
					break; 
			} 

			// if p == t and pat[0...M-1] = txt[i, i+1, ...i+M-1] 
			if (j == M){
				if(r[0]+1 > dim){
					dim = dim*2;
					r = realloc (r, dim * sizeof(long));
					if(!r){
						printf("Error realloc");
						return NULL;
					}
				}
			
				r[r[0]+1] = i;
				r[0] += 1;
			} 
		} 

		// Calculate hash value for next window of text: Remove 
		// leading digit, add trailing digit 
		if ( i < N-M ) 
		{ 
			t = (NO_OF_CHARS*(t - txt[i]*h) + txt[i+M])%q; 

			// We might get negative value of t, converting it 
			// to positive 
			if (t < 0) 
				t = (t + q); 
		} 
	} 
	
	r = realloc (r, (r[0]+1) * sizeof(long));
	if(!r){
		printf("Error realloc finale\n");
		return NULL;
	}
	return r;
} 

/* Driver Code */
int main(){

	char path[] = "dataset/dataset5.txt";
	char* txt;
	int n=0;
	int q = 101;
	FILE *fin;

	if((fin = fopen(path, "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}
	
	double t1 = omp_get_wtime();		
	txt = readFile(fin, &n);	
	double t2 = omp_get_wtime();
	printf("T lettura:%f\n",t2-t1);
	fclose(fin);

	
	t1 = omp_get_wtime();		
	long *r = search("ACATACACAAA",txt,q);
	t2 = omp_get_wtime();
	printf("T cerca:%f\n",t2-t1);
	
	if(r == NULL){
		printf("NUll");
		return;
	}
	
	printf("D:%ld",r[r[0]]);
	
	return 0;
}
