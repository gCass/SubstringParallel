#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
  
char * readFile(FILE *fin, int *n);

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
			//Non necessaria sezione critica su c perche' a ogni iterazione scrivo una cella diversa
			// c e' solo un vettore di puntatori. Quello che noi andiamo a modificare in realta' e' z, che contiene i valori della matrice
			// Se noi facciamo in modo che ogni thread abbia la sua copia di c FORSE e' piu' veloce
			#pragma omp parallel for shared(a,b) private(i,j) firstprivate(c) schedule(static, 1024)
			for (l=1; l <= MIN(m,k); l++){				
				//printf("l:%d\n",l);

				j = l;
				i = k-l+1;//Io voglio sempre i+j=k+1
				
//				printf("i:%d\n",i);
//				printf("j:%d\n",j);
//				printf("\n\n");

						
    			if (a[i - 1] == b[j - 1]) {
    			  
             	  c[i][j] = c[i - 1][j - 1] + 1;
            	}
            	else {
             	   c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            	}
			}	
		}else {
//			printf("Sotto\n");
			#pragma omp parallel for shared(a,b) private(i,j) firstprivate(c) schedule(static, 1024)
			for (l=k-n+1; l <= m; l++){
    			//printf("l:%d\n",l);

    			j = l;
				i = k-l+1; //Io voglio sempre i+j=k+1
				
//				printf("j:%d\n",j);
//				printf("i:%d\n",i);
//				printf("\n\n");
    			
    			if (a[i - 1] == b[j - 1]) {
             	  c[i][j] = c[i - 1][j - 1] + 1;
            	}
            	else {
             	   c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
            	}
    			
    			
			}
		}
		 	
	}
//	printf("Matrice c\n");
//	for(i=0; i<=n; i++){
//		for(j=0; j<=m; j++){
//			printf("%d ",c[i][j]);
//		}
//		printf("\n");
//	}
    

    //t contiene il valore finale della lunghezza della lcs
    t = c[n][m];
    //Alloco lo spazio per la lcs
    *s = malloc(t*sizeof(char));
    //Ricostruisco la lcs
    for (i = n, j = m, k = t - 1; k >= 0;) {
        if (a[i - 1] == b[j - 1])
            (*s)[k] = a[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            j--;
        else
            i--;
    }
    //Aggiungo il carattere terminatore alla fine della stringa
	(*s)[t] = '\0';
	
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
    
    omp_set_num_threads(2);
    
    // Come parametri vogliamo i nomi dei due file e la dimensione del blocco.
    // Tra i parametri viene sempre considerato anche il nome del programma
    if(argc != 3) {
    	printf("Inserire come parametri del programma i nomi dei due file \n");
    	return 0;
	}
	
	//BLOCK_SIZE = atoi(argv[3]);
    
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
    /*
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
	*/
    
    t1 = omp_get_wtime();

    int t = lcs(a, n, b, m, &s);
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
