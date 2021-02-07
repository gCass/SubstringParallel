#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
 
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int BLOCK_SIZE;

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
    int *z = calloc((n + 1) * (m + 1), sizeof (int));
    int **c = calloc((n + 1), sizeof (int *));
    double t1,t2;
    
    //Pensare a fugnata per aggiungere caratteri in modo da fare che le stringhe
    //sono multiple del block size
    //TODO...
    
	//Assegno a c i puntatori che puntano alle varie posizioni di z (vedere disegno su paint)
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
    //Dividere la matrice in blocchi
    //int BLOCK_SIZE = 16;
//    int number_of_blocks = MIN(n/5, m/5);
//    printf("Number of blocks:%d\n",number_of_blocks);
    
    //Dynamic programming che mette nella matrice i vari risultati di lcs ecc...
	//ciclo tra blocchi diversi, per ora di grandezza 5
	k = 0;
	
	//We need to get the correct index for the blocks
	int i_start;
	int j_start;
	
	
	//Da controllare l'estremo finale
	int l;
	t1 = omp_get_wtime();
	for (k=1; k <= (m+n)/BLOCK_SIZE - 1; k++){
    	
		//printf("Sopra K:%d",k);
		//Ora seleziono quale blocco interno alla diagonale di blocchi
		//Questi sono i cicli indipendenti da parallelizzare
		for (l=MAX(1,k-n/BLOCK_SIZE+1); l <= MIN(m/BLOCK_SIZE,k); l++){				
			//Qua dentro devo applicare calcolo della matrice normale 
			//k=1, l=1 => i=1, j=1
			//k=2, l=1 => i=1+BLOCK*(K-l)=1+B*(2-1), j=1+BLOCK*(l-1)
			//k=2, l=2 => i=1+BLOCK*(K-l)=1, j=1+BLOCK*(2-1)=1+BLOCK 
			//k=3, l=1 => i=1+BLOCK*(3-1), j=1=1+BLOCK*(1-1)
			//k=3, l=2 => i=1+BLOCK*(3-2), j=1+BLOCK*(2-1)=1+BLOCK
			//k=3, l=3 => i=1+BLOCK*(3-3), j=1+2*BLOCK=1+BLOCK()
			
			i_start = 1+BLOCK_SIZE*(k-l);				
			j_start = 1+BLOCK_SIZE*(l-1);

//				printf("i_start:%d\n",i_start);
//				printf("j_start:%d\n",j_start);
//				printf("\n");
			
			//Applico il lcs seriale
			//					<= n
			for (i = i_start; i < i_start+BLOCK_SIZE; i++) {
									//<= m
				for (j = j_start; j < j_start+BLOCK_SIZE; j++) {
		        	
//						printf("i:%d\n",i);
//						printf("j:%d\n",j);
//						printf("\n\n");
//			        	
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
    t2 = omp_get_wtime();
	printf("T algoritmo vero lcs:%f\n",t2-t1);

    
//    printf("Matrice c\n");
//	for(i=0; i<=n; i++){
//		for(j=0; j<=m; j++){
//			printf("%d ",c[i][j]);
//		}
//		printf("\n");
//	}
    
    //t contiene il valore finale della lunghezza della lcs
    t = c[n][m];
    //Alloco lo spazio per la lcs
    *s = malloc(t);
    //Ricostruisco la lcs
    t1 = omp_get_wtime();
    for (i = n, j = m, k = t - 1; k >= 0;) {
        if (a[i - 1] == b[j - 1])
            (*s)[k] = a[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            j--;
        else
            i--;
    }
    t2 = omp_get_wtime();
	printf("T ricostruzione:%f\n",t2-t1);
    
    //Libero lo spazio
    
    t1 = omp_get_wtime();
    free(c);
    free(z);
    t2 = omp_get_wtime();
	printf("T free:%f\n",t2-t1);
    return t;
}

int main (int argc, char *argv[]) {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    double t1,t2;
    //char path1[256], path2[256];	// I percorsi dei due file non possono superare i 256 caratteri
    
    // Come parametri vogliamo i nomi dei due file e la dimensione del blocco.
    // Tra i parametri viene sempre considerato anche il nome del programma
    if(argc != 4) {
    	printf("Inserire come parametri del programma i nomi dei due file e la dimensione del blocco\n");
    	return 0;
	}
	
//	char path1[] = "dataset/stringa_lcs_3.txt";
//	char path2[] = "dataset/stringa_lcs_4.txt";	

	BLOCK_SIZE = atoi(argv[3]);
	
	FILE *fin;

	if((fin = fopen(argv[1], "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}
	t1 = omp_get_wtime();	
	a = readFile(fin, &N);
    fclose(fin);
    
    if((fin = fopen(argv[2], "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}	
	b = readFile(fin, &N);
    fclose(fin);
 	t2 = omp_get_wtime();
	printf("T lettura:%f\n",t2-t1);
	
	//Faccio una 20x15, così da fare blocchi da 5
	
//	a = "UUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAO";
//	b = "PROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVA";
//    

     //I primi 3 caratteri del file sono caratteri di controllo
    //li scartiamo facendo +3 al puntatore
    int n = strlen(a+3);
    int m = strlen(b+3);
    
    
    t1 = omp_get_wtime();

    int t = lcs(a+3, n, b+3, m, &s);
    t2 = omp_get_wtime();
	printf("T lcs:%f\n",t2-t1);
    
//	printf(a);
//	printf("\n");
//	
//	printf(b);
//	printf("\n");
	printf("%d\n",t);
	printf("%.*s\n", t, s); // tsitest
    return 0;
}
