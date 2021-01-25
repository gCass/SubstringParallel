// Knuth Morris Pratt algorithm
// Source (C++ code): https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/

// Questo algoritmo ottimizza cercando nel pattern i prefissi che sono anche suffissi
// Nel caso di stringhe genetiche questa ottimizzazione e' particolarmente utile
// perche' essendoci solo 4 caratteri disponibili e' piu' frequente avere sottostringhe che si ripetono
// rispetto a un testo normale

// C++ program for implementation of KMP pattern searching 
// algorithm 
#include<stdio.h> 
#include<stdlib.h>
#include<string.h>  
#include<omp.h>

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

void computeLPSArray(char* pat, int M, int* lps); 

// Prints occurrences of txt[] in pat[] 
long* search(char* pat, char* txt) 
{ 
	long M = strlen(pat); 
	long N = strlen(txt); 
	
	long* r; //r[0] contiene il numero di elementi del vettore 
	int dim=20;
	r = malloc(dim * sizeof(long));
	
	if(!r){
		printf("Error malloc");
		return NULL;
	}
	
	r[0] = 0;

	// create lps[] that will hold the longest prefix suffix 
	// values for pattern 
	int lps[M]; 

	// Preprocess the pattern (calculate lps[] array) 
	computeLPSArray(pat, M, lps); 

	int i = 0; // index for txt[] 
	int j = 0; // index for pat[] 
	while (i < N) { 
		if (pat[j] == txt[i]) { 
			j++; 
			i++; 
		} 

		if (j == M) { 
			//printf("Found pattern at index %d ", i - j);
			if(r[0]+1 > dim)
			{
				dim = dim*2;
				r = realloc (r, dim * sizeof(long));
				if(!r){
					printf("Error realloc");
					return NULL;
				}
			}
			r[r[0]+1] = i;
			r[0] += 1;
			
			j = lps[j - 1]; 
		} 
		// mismatch after j matches 
		else if (i < N && pat[j] != txt[i]) { 
			// Do not match lps[0..lps[j-1]] characters, 
			// they will match anyway 
			if (j != 0) 
				j = lps[j - 1]; 
			else
				i = i + 1; 
		} 
	}
	r = realloc (r, (r[0]+1) * sizeof(long));
	if(!r){
		printf("Error realloc finale\n");
		return NULL;
	}
	return r; 
} 

// Fills lps[] for given patttern pat[0..M-1] 
void computeLPSArray(char* pat, int M, int* lps) 
{ 
	// length of the previous longest prefix suffix 
	int len = 0; 

	lps[0] = 0; // lps[0] is always 0 

	// the loop calculates lps[i] for i = 1 to M-1 
	int i = 1; 
	while (i < M) { 
		if (pat[i] == pat[len]) { 
			len++; 
			lps[i] = len; 
			i++; 
		} 
		else // (pat[i] != pat[len]) 
		{ 
			// This is tricky. Consider the example. 
			// AAACAAAA and i = 7. The idea is similar 
			// to search step. 
			if (len != 0) { 
				len = lps[len - 1]; 

				// Also, note that we do not increment 
				// i here 
			} 
			else // if (len == 0) 
			{ 
				lps[i] = 0; 
				i++; 
			} 
		} 
	} 
} 

// Driver program to test above function 
int main() 
{ 
	
	char path[] = "dataset/dataset5.txt";
	char* txt;
	int n=0;
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
	long *r = search("ACATACACAAA",txt);
	t2 = omp_get_wtime();
	printf("T cerca:%f\n",t2-t1);
	
	if(r == NULL){
		printf("NUll");
		return;
	}
	
	printf("D:%ld",r[r[0]]);
	
	return 0;
} 

