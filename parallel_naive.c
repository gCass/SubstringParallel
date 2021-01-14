//Prova 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


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

long* search(char* pat, char* txt) 
{ 
	long M = strlen(pat); 
	long N = strlen(txt); 
	long* r; //r[0] contiene il numero di elementi del vettore 
	int dim=20;
	long i;	

	
	/* A loop to slide pat[] one by one */
	
	r = malloc(dim * sizeof(long));
	
	if(!r){
		printf("Error malloc");
		return NULL;
	}
	
	r[0] = 0;
	
	#pragma omp parallel shared(r,dim)
	{	
		#pragma omp single
		{
			int n_thread = omp_get_num_threads();
			printf("N thread:%d\n", n_thread);
		}
		
		#pragma omp for	
		for (i = 0; i <= N - M; i++) { 
			long j; 
	
			/* For current index i, check for pattern match */
			for (j = 0; j < M; j++) 
				if (txt[i + j] != pat[j]) 
					break; 
	
			if (j == M){
				//printf("Pattern found at index %d \n", i); 
				#pragma omp critical
				{
					if(r[0]+1 > dim){
						dim = dim*2;
						r = realloc (r, dim * sizeof(long));
						if(!r){
							printf("Error realloc");
							//return NULL;
						}
					}
				
					r[r[0]+1] = i;
					r[0] += 1;
					//printf("R[0]:%d\n",r[0]);
				}	
			} // if pat[0...M-1] = txt[i, i+1, ...i+M-1] 
				
		} 
	}

	r = realloc (r, (r[0]+1) * sizeof(long));
	if(!r){
		printf("Error realloc finale\n");
		return NULL;
	}
	return r;
} 



int main(){

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
