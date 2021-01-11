#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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



int main(){

	//char path[] = "dataset/dna_corto_pulito.txt";
	char path[] = "dataset/ManuSporny-genome.txt";
	char* txt;
	int n=0;
	FILE *fin;

	if((fin = fopen(path, "r"))==NULL){
		printf("Errore nell'apertura del file!");
		return -1;
	}
	
	txt = readFile(fin, &n);
	//printf("%s",txt);
	return 0;


}
