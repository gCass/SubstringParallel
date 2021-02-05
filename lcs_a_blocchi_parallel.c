#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

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
    
	//Assegno a c i puntatori che puntano alle varie posizioni di z (vedere disegno su paint)
	for (i = 0; i <= n; i++) {
        c[i] = &z[i * (m + 1)];
    }
    
    //Dividere la matrice in blocchi
    int BLOCK_SIZE = 16;
//    int number_of_blocks = MIN(n/5, m/5);
//    printf("Number of blocks:%d\n",number_of_blocks);
    
    //Dynamic programming che mette nella matrice i vari risultati di lcs ecc...
	//ciclo tra blocchi diversi, per ora di grandezza 5
	k = 0;
	
	//We need to get the correct index for the blocks
	int row_start;
	int column_start;
	
	
	//Da controllare l'estremo finale
	int l;
	//Itero selezionato un numero di blocchi sulle diagonali
	for (k=1; k <= (m+n)/BLOCK_SIZE - 1; k++){
    	
		
		if(k < (n/BLOCK_SIZE)+1){
    		//sopra
    		//printf("Sopra K:%d",k);
    		//Ora seleziono quale blocco interno alla diagonale di blocchi
    		//Questi sono i cicli indipendenti da parallelizzare
    		
    		#pragma omp parallel for private(i,j)
			for (l=1; l <= MIN(m/BLOCK_SIZE,k); l++){				
				//Qua dentro devo applicare calcolo della matrice normale 
				//k=1, l=1 => i=1, j=1
				//k=2, l=1 => i=1+BLOCK*(K-l)=1+B*(2-1), j=1+BLOCK*(l-1)
				//k=2, l=2 => i=1+BLOCK*(K-l)=1, j=1+BLOCK*(2-1)=1+BLOCK 
				//k=3, l=1 => i=1+BLOCK*(3-1), j=1=1+BLOCK*(1-1)
				//k=3, l=2 => i=1+BLOCK*(3-2), j=1+BLOCK*(2-1)=1+BLOCK
				//k=3, l=3 => i=1+BLOCK*(3-3), j=1+2*BLOCK=1+BLOCK()
				
				int i_start = 1+BLOCK_SIZE*(k-l);				
				int j_start = 1+BLOCK_SIZE*(l-1);

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
						//Quando accediamo le celle di una matrice di blocchi diversi in parallelo
						//non entriamo mai in parallelo sulla stessa cella, quindi la sezione critica non serve
			            if (a[i - 1] == b[j - 1]) {			            	
			                c[i][j] = c[i - 1][j - 1] + 1;
			            }
			            else {
			                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
			            }
			        }
			    }				
			}			
		}else{
			//sotto
			//printf("Sotto K:%d",k);
			//Ora seleziono quale blocco interno alla diagonale di blocchi
    		#pragma omp parallel for private(i,j)
			for (l=k-n/BLOCK_SIZE+1; l <= MIN(m/BLOCK_SIZE,k); l++){	
				
				//k=4, l=1 => i=1+BLOCK*(K-l)=1+B*(2-1), j=1+BLOCK*(l-1)
				//k=4, l=2 => i=1+BLOCK*(K-l)=1, j=1+BLOCK*(2-1)=1+BLOCK 
				//k=4, l=1 => i=1+BLOCK*(3-1), j=1=1+BLOCK*(1-1)
				//k=4, l=2 => i=1+BLOCK*(3-2), j=1+BLOCK*(2-1)=1+BLOCK
				//k=5, l=3 => i=1+BLOCK*(3-3), j=1+2*BLOCK=1+BLOCK()
				
//				printf("L:%d\n",l);
				
				int i_start = 1+BLOCK_SIZE*(k-l);				
				int j_start = 1+BLOCK_SIZE*(l-1);
				
//				printf("i_start:%d\n",i_start);
//				printf("j_start:%d\n",j_start);
//				printf("\n");
//				
				
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
	}
    
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
    
	//Ricostruisco la lcs (per ora non parallelizabile, magari da ricostruire in modo parallelizzato)
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
    double t1,t2;
    
  	char path1[] = "dataset/stringa_lcs_3.txt";
	char path2[] = "dataset/stringa_lcs_4.txt";	
	FILE *fin;
	int reading_error = 0; //Flag gestione errore lettura file
	
	t1 = omp_get_wtime();
	
	//La lettura dei due file in parallelo è un'aggiunta in più, ma che non ha grandi vantaggi 	
	#pragma omp parallel private(fin)
	{	
		#pragma omp single
		{
			#pragma omp task
			{
				if((fin = fopen(path1, "r"))!=NULL){
//					printf("Errore nell'apertura del file!");					
					a = readFile(fin, &N);
					fclose(fin);						
				}else{
					reading_error = 1;
				}				
			}
			
			#pragma omp task
			{
				if((fin = fopen(path1, "r"))!=NULL){
//					printf("Errore nell'apertura del file!");					
					b = readFile(fin, &N);
					fclose(fin);						
				}else{
					reading_error = 1;
				}	
			}
			
			#pragma omp taskwait	
		}
	}
	
 	t2 = omp_get_wtime();
	printf("T lettura:%f\n",t2-t1);

	if(reading_error == 1){
		printf("Errore nell'apertura del file!");
		return -1;
	}


	//Faccio una 15x15, così da fare blocchi da 5
	
//	a = "UUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAO";
//	b = "PROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVA";
    

    //I primi 3 caratteri del file sono caratteri di controllo
    //li scartiamo facendo +3 al puntatore
    int n = strlen(a+3);
    int m = strlen(b+3);
    printf("n:%d m:%d\n",n,m);
    
    t1 = omp_get_wtime();

    int t = lcs(a+3, n, b+3, m, &s);
    t2 = omp_get_wtime();
	printf("T lcs:%f\n",t2-t1);
	
//	printf(a);
//	printf("\n");
//	
//	printf(b);
//	printf("\n");
	
	printf("%.*s\n", t, s); // tsitest
    return 0;
}
