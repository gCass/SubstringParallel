#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int BLOCK_SIZE;

// Leggo il file a blocchi della dimensione del blocco che poi usero' per l'algoritmo
// n e' il numero di blocchi che ho letto
// Passo anche il numero di caratteri che devo scartare all'inizio del file
char * readFile(FILE *fin, int *n) {
	int BLOCK_READ_SIZE = BLOCK_SIZE;
	char * txt, tmp;
	int dim=8, read;
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
	//Itero selezionato un numero di blocchi sulle diagonali
	for (k=1; k <= (m+n)/BLOCK_SIZE - 1; k++){
    	
		
		if(k < (n/BLOCK_SIZE)+1){
    		//sopra
    		//printf("Sopra K:%d",k);
    		//Ora seleziono quale blocco interno alla diagonale di blocchi
    		//Questi sono i cicli indipendenti da parallelizzare
    		
    		#pragma omp parallel for private(i,j, i_start, j_start)
			for (l=1; l <= MIN(m/BLOCK_SIZE,k); l++){				
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
    		#pragma omp parallel for private(i,j, i_start, j_start)
			for (l=k-n/BLOCK_SIZE+1; l <= MIN(m/BLOCK_SIZE,k); l++){	
				
				//k=4, l=1 => i=1+BLOCK*(K-l)=1+B*(2-1), j=1+BLOCK*(l-1)
				//k=4, l=2 => i=1+BLOCK*(K-l)=1, j=1+BLOCK*(2-1)=1+BLOCK 
				//k=4, l=1 => i=1+BLOCK*(3-1), j=1=1+BLOCK*(1-1)
				//k=4, l=2 => i=1+BLOCK*(3-2), j=1+BLOCK*(2-1)=1+BLOCK
				//k=5, l=3 => i=1+BLOCK*(3-3), j=1+2*BLOCK=1+BLOCK()
				
//				printf("L:%d\n",l);
				
				i_start = 1+BLOCK_SIZE*(k-l);				
				j_start = 1+BLOCK_SIZE*(l-1);
				
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

int main (int argc, char *argv[]) {
    char *a;
    char *b;
   	int N=0;
    char *s = NULL;
    double t1,t2;
    
    omp_set_num_threads(24);
    
    // Come parametri vogliamo i nomi dei due file e la dimensione del blocco.
    // Tra i parametri viene sempre considerato anche il nome del programma
    if(argc != 4) {
    	printf("Inserire come parametri del programma i nomi dei due file e la dimensione del blocco\n");
    	return 0;
	}
	
	BLOCK_SIZE = atoi(argv[3]);
    
  	//char path1[] = "dataset/stringa_lcs_3.txt";
	//char path2[] = "dataset/stringa_lcs_4.txt";	
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
				if((fin = fopen(argv[1], "r"))!=NULL){
//					printf("Errore nell'apertura del file!");					
					a = readFile(fin, &N);
					fclose(fin);						
				}else{
					reading_error = 1;
				}				
			}
			
			#pragma omp task
			{
				if((fin = fopen(argv[2], "r"))!=NULL){
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
	printf("T_lettura %f\n",t2-t1);

	if(reading_error == 1){
		printf("Errore nell'apertura del file!");
		return -1;
	}


	//Faccio una 15x15, così da fare blocchi da 5
	
//	a = "UUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAOUUUAO";
//	b = "PROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVAPROVACAZZOPROVA";
    

    //I primi 3 caratteri del file sono caratteri di controllo
    //li scartiamo facendo +3 al puntatore
   int n = strlen(a);
   int m = strlen(b);
   //printf("n: %d\n",n);
   //printf("m: %d\n",m);
    
    int n_tondo;
    if(n % BLOCK_SIZE == 0){
    	n_tondo = n;
	} else {
		n_tondo = BLOCK_SIZE * (n/BLOCK_SIZE + 1);
	}
	//printf("n_tondo: %d\n",n_tondo);
    
    int i;
    for(i=n; i<n_tondo; i++){
    	a[i] = '1';	// Aggiungo caratteri numerici che non compariranno mai in stringhe genetiche
	}
	
	int m_tondo;
    if(m % BLOCK_SIZE == 0){
    	m_tondo = m;
	} else {
		m_tondo = BLOCK_SIZE * (m/BLOCK_SIZE + 1);
	}
	//printf("m_tondo: %d\n",m_tondo);
    
    for(i=m; i<m_tondo; i++){
    	b[i] = '2';	// Aggiungo caratteri numerici che non compariranno mai in stringhe genetiche
	}
    
    t1 = omp_get_wtime();

    int t = lcs(a, n_tondo, b, m_tondo, &s);
    t2 = omp_get_wtime();
	printf("T_lcs %f\n",t2-t1);
	
//	printf(a);
//	printf("\n");
//	
//	printf(b);
//	printf("\n");
	//printf("%d\n",t);
	//printf("%.*s\n", t, s); // tsitest
    return 0;
}
