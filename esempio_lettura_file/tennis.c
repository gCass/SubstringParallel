#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAXCARATTERI (50)
#define MAXSET 5

struct partita_t {
	char gioc1[MAXCARATTERI + 1];
	char gioc2[MAXCARATTERI + 1];
	int nset;
	int set_giocati;
	int punti[MAXSET][2];
};

struct giocatore_t {
	char nome[MAXCARATTERI + 1];
	int set_vinti;
};

struct partita_t * leggi_file (FILE *fin, int *n);
int * giochi_per_partita (struct partita_t *partite, int n);
int max (int v[], int n);   /*ritorna l'indice del valore massimo in un vettore di interi*/
void stampa_partite(struct partita_t *partite, int n);
void medie (struct partita_t *partite, int n);
int conta_tie (struct partita_t *partite, int n);
struct giocatore_t * riempi_giocatori(struct partita_t *partite, int n_partite, int *n_gioc);
int cerca_giocatore (char nome[], struct giocatore_t *giocatori, int n);
int confronta (const void *g1, const void *g2);

int main (int argc, char *argv[]) {
	FILE *fin;
	struct partita_t *partite;
	int n_partite, *giochi, pmg; //pmg->partita max giochi
	int tot=0, i, n_gioc;
	struct giocatore_t *giocatori;
	
	if(argc!=2) {
		printf("Inserire il nome del file come parametro\n");
		return 1;
	}
	if(!(fin = fopen(argv[1], "r"))) {
		printf("Errore in apertura del file\n");
		return 1;
	}
	partite = leggi_file(fin, &n_partite);
	if(partite == NULL) {
		printf("Errore in letttura file\n");
		return 1;
	}
	//stampa_partite(partite, n_partite);
	
	giochi = giochi_per_partita(partite, n_partite);
	pmg = max (giochi, n_partite);
	printf("[MAX-GIOCHI]\n%s %s %d\n", partite[pmg].gioc1, partite[pmg].gioc2, giochi[pmg]);
	for (i=0; i<n_partite; i++)
		tot += giochi[i];
	printf("[TOT-GIOCHI]\n%d\n", tot);
	printf("[MEDIA]\n");
	medie(partite, n_partite);
	printf("[TIE]\n%d\n", conta_tie(partite, n_partite));
	giocatori = riempi_giocatori(partite, n_partite, &n_gioc);
	printf("[UTENTI]\n");
	for(i=0; i<n_gioc; i++)
		printf("%s\n", giocatori[i].nome);
	qsort(giocatori, n_gioc, sizeof(*giocatori), confronta);
	printf("[CLASSIFICA]\n");
	if (n_gioc<10) {
		for (i=0; i<n_gioc; i++) {
			printf("%s\n", giocatori[i].nome);
		}
	} else {
		for (i=0; i<n_gioc; i++) {
			printf("%s %d\n", giocatori[i].nome, giocatori[i].set_vinti);
		}
	}
	
	free(giocatori);
	free(partite);
	fclose(fin);
	return 0;
}

struct partita_t * leggi_file (FILE *fin, int *n) {
	struct partita_t *partite, tmp;
	int dim=8, read, i, j;
	char buf[200];
	(*n)=0;
		
	partite = malloc (dim * sizeof(*partite));
	if(!partite) {
		return NULL;
	}
	while(fgets(buf, sizeof(buf), fin)) {
		read = sscanf(buf, "%s %s %d %d-%d %d-%d %d-%d %d-%d %d-%d", &tmp.gioc1, &tmp.gioc2, &tmp.nset, &tmp.punti[0][0], &tmp.punti[0][1], &tmp.punti[1][0], &tmp.punti[1][1], &tmp.punti[2][0], &tmp.punti[2][1], &tmp.punti[3][0], &tmp.punti[3][1], &tmp.punti[4][0], &tmp.punti[4][1]);
		partite[(*n)].set_giocati = (read - 3)/2;
		strcpy(partite[(*n)].gioc1, tmp.gioc1);
		strcpy(partite[(*n)].gioc2, tmp.gioc2);
		partite[(*n)].nset = tmp.nset;
		for (i=0; i<((read - 3)/2); i++) {
			for (j=0; j<2; j++) {
				partite[(*n)].punti[i][j] = tmp.punti[i][j];
			}
		}
		
		(*n)++;
		if((*n)>=dim) {
			dim *= 2;
			partite = realloc (partite, dim * sizeof(*partite));
			if(!partite)
				return NULL;
		}
	}
	partite = realloc (partite, (*n) * sizeof(*partite));
	
	return partite;
}

int * giochi_per_partita (struct partita_t *partite, int n) {
	int i, j, *giochi, sum;
	giochi = calloc (n, sizeof(*giochi));
	for(i=0; i<n; i++)
		giochi[i] = 0;
	
	for (i=0; i<n; i++) {
		sum=0;
		for (j=0; j<partite[i].set_giocati; j++)
			sum += (partite[i].punti[j][0] + partite[i].punti[j][1]);
		giochi[i]=sum;
	}
	
	return giochi;
}

int max (int v[], int n) {
	int i, val_max=0, ind_max;
	
	for (i=0; i<n; i++) {
		if(v[i]>val_max) {
			val_max=v[i];
			ind_max=i;
		}
	}
	return ind_max;
}

void stampa_partite(struct partita_t *partite, int n) {
	int i, j;
	for(i=0; i<n; i++) {
		printf("Partita %d: %s vs %s, %d set_max, %d set giocati\n", i, partite[i].gioc1, partite[i].gioc2, partite[i].nset, partite[i].set_giocati);
		printf("punteggi: ");
		for (j=0; j<partite[i].set_giocati; j++) {
			printf("%d-%d ", partite[i].punti[j][0], partite[i].punti[j][1]);
		}
		printf("\n");
	}
}

void medie (struct partita_t *partite, int n) {
	int i, sum3=0, sum5=0, conta3=0, conta5=0;
	
	for (i=0; i<n; i++) {
		if(partite[i].nset == 3) {
			conta3++;
			sum3 += partite[i].set_giocati;
		} else {
			conta5++;
			sum5 += partite[i].set_giocati;
		}
	}
	if (conta3 == 0)
		printf("NULL\n");
	else
		printf("%f\n", sum3/(double)(conta3));
	if (conta5 == 0)
		printf("NULL\n");
	else
		printf("%f\n", sum5/(double)(conta5));
	
	
}

int conta_tie (struct partita_t *partite, int n) {
	int n_tie=0, i, j;
	
	for (i=0; i<n; i++) {
		for (j=0; j<partite[i].set_giocati; j++) {
			if ((partite[i].punti[j][0] == 7) || (partite[i].punti[j][1] == 7))
				n_tie++;
		}
	}
	return n_tie;
}

struct giocatore_t * riempi_giocatori(struct partita_t *partite, int n_partite, int *n_gioc) {
	int i, dim=8, gioc_attuale, j;
	struct giocatore_t *giocatori;
	
	giocatori = malloc(dim * sizeof(*giocatori));
	(*n_gioc) = 0;
	for(i=0; i<n_partite; i++) {
		//lettura gioc1
		gioc_attuale = cerca_giocatore(partite[i].gioc1, giocatori, (*n_gioc));
		if(gioc_attuale==-1) {
			gioc_attuale = (*n_gioc);
			strcpy(giocatori[gioc_attuale].nome, partite[i].gioc1);
			(*n_gioc)++;
			giocatori[gioc_attuale].set_vinti = 0;
		}
		for (j=0; j<partite[i].set_giocati; j++) {
			if (partite[i].punti[j][0]>partite[i].punti[j][1])
				giocatori[gioc_attuale].set_vinti ++;
		}
		if((*n_gioc)>=dim) {
			dim *= 2;
			giocatori = realloc (giocatori, dim * sizeof(*giocatori));
		}
		//lettura gioc2
		gioc_attuale = cerca_giocatore(partite[i].gioc2, giocatori, (*n_gioc));
		if(gioc_attuale==-1) {
			gioc_attuale = (*n_gioc);
			strcpy(giocatori[gioc_attuale].nome, partite[i].gioc2);
			(*n_gioc)++;
			giocatori[gioc_attuale].set_vinti = 0;
		}
		for (j=0; j<partite[i].set_giocati; j++) {
			if (partite[i].punti[j][1]>partite[i].punti[j][0])
				giocatori[gioc_attuale].set_vinti ++;
		}
		if((*n_gioc)>=dim) {
			dim *= 2;
			giocatori = realloc (giocatori, dim * sizeof(*giocatori));
		}
	}
	giocatori = realloc (giocatori, (*n_gioc) * sizeof(*giocatori));
	return giocatori;
}

int cerca_giocatore (char nome[], struct giocatore_t *giocatori, int n) {
	int i;
	for(i=0; i<n; i++)
		if((strcmp(nome, giocatori[i].nome))==0) {
			//printf("Giocatore %s indice %d\n", nome, i);
			return i;
		}
	//printf("Giocatore %s non trovato\n", nome);
	return -1;
}

int confronta (const void *g1, const void *g2) {
	const struct giocatore_t *gioc1 = g1, *gioc2 = g2;
	if(gioc1->set_vinti<gioc2->set_vinti)
		return 1;
	else if (gioc1->set_vinti>gioc2->set_vinti)
		return -1;
	else
		return (strcmp(gioc1->nome, gioc2->nome));
}



