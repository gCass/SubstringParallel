#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int BLOCK_SIZE;

//Function to read a file 
char * readFile(FILE *fin, int *n) {
	int BLOCK_READ_SIZE = BLOCK_SIZE;
	char * txt, tmp;
	int dim=8, read;
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
    int l;
	int i_start;
	int j_start;	
	int *z = calloc((n + 1) * (m + 1), sizeof (int));
    int **c = calloc((n + 1), sizeof (int *));
    
	//Assign to variable c the pointers to the different rows of the matrix 
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
	for (k=1; k <= (m+n)/BLOCK_SIZE - 1; k++){   	    		
		#pragma omp parallel for private(i,j, i_start, j_start)
		for (l=MAX(1,k-n/BLOCK_SIZE+1); l <= MIN(m/BLOCK_SIZE,k); l++){				
			
			i_start = 1+BLOCK_SIZE*(k-l);				
			j_start = 1+BLOCK_SIZE*(l-1);

			//Here we apply the serial lcs algorithm to the block
			for (i = i_start; i < i_start+BLOCK_SIZE; i++) {
				for (j = j_start; j < j_start+BLOCK_SIZE; j++) {
		        				        	
		            //Since we never access matrix cells of same block in parallel, critic section isn't necessary
					if (a[i - 1] == b[j - 1]) {			            	
		                c[i][j] = c[i - 1][j - 1] + 1;
		            }
		            else {
		                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
		            }
		        }
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
    
    omp_set_num_threads(24);
    
    
    //The parameters are the 2 file paths and the size of the block.
    if(argc != 4) {
    	printf("Inserire come parametri del programma i nomi dei due file e la dimensione del blocco\n");
    	return 0;
	}
	
	BLOCK_SIZE = atoi(argv[3]);
		
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
    
    //Round the dimension of first string as multiple of the block size
    int n_tondo;
    if(n % BLOCK_SIZE == 0){
    	n_tondo = n;
	} else {
		n_tondo = BLOCK_SIZE * (n/BLOCK_SIZE + 1);
	}
    	
    //Add numeric characters that never appear in genetics strings into first string
    int i;
    for(i=n; i < n_tondo; i++){
    	a[i] = '1';	
	}
	
	//Round the dimension of second string as multiple of the block size
	int m_tondo;
    if(m % BLOCK_SIZE == 0){
    	m_tondo = m;
	} else {
		m_tondo = BLOCK_SIZE * (m/BLOCK_SIZE + 1);
	}
    
    //Add numeric characters that never appear in genetics strings into second string
    for(i=m; i<m_tondo; i++){
    	b[i] = '2';	
	}
    
    t1 = omp_get_wtime();

    int t = lcs(a, n_tondo, b, m_tondo, &s);
    t2 = omp_get_wtime();
	printf("T_lcs %f\n",t2-t1);
	
	//Print length of lcs string
	//printf("%d\n",t);
	
	//Print the lcs string
	//printf("%.*s\n", t, s); // tsitest
    return 0;
}
