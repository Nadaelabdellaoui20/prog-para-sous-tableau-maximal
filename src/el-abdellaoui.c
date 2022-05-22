
#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include<stdbool.h>
#include <omp.h>
//#include<time.h>



// pour bien comprendre les erreurs de valgrind: https://www.cprogramming.com/debugging/valgrind.html

struct tableau
{
	long long *current;
	int size;
	
};

struct tableau * create_tableau(int size){
	//    struct tableau * destination = malloc(sizeof(struct tableau));

	struct tableau * tab = malloc(sizeof(struct tableau ));
	tab->size= size;
   	tab->current = malloc(tab->size*sizeof(long long int)); 
   	tab->current[0]=0;
   	return tab;
}

long long maximum(long long x, long long y){
	return (x > y ) ? x : y;
}

/* PREFIX SUM*/

// https://stackoverflow.com/questions/1448318/omp-parallel-vs-omp-parallel-for

//Parallel
void montee(struct tableau *firsttable , struct tableau *climbing)
{
	for (int i=0 ; i< firsttable->size ;i++){ 
		climbing->current[i+firsttable->size] = firsttable->current[i];
	}
	for (int l=(log(firsttable->size)/log(2))-1;l >= 0; l--){ 
		int max=pow(2,l+1);
		#pragma omp parallel for
		for(int j= pow(2,l); j<max;j++){ 
			climbing->current[j]= climbing->current[(2*j)]+ climbing->current[(2*j)+1];
		}
	}

}

//Parallel
void descente(struct tableau *climbing , struct tableau *descent){
	descent->current[1]=0; 
	for (int l= 1; l<(log(climbing->size)/log(2));l++){ 
		int max=pow(2,l+1); 
		#pragma omp parallel for
		for (int j = pow(2,l); j <max; j++) 
		{
			if ((j)%2 == 0) 	//even
			{
                descent->current[j] = descent->current[j/2];
            }
            else { 			//odd
                descent->current[j] = descent->current[(j)/2] + climbing->current[j-1];
            }
		}
	}
	

}

//Parallel
void finale(struct tableau *montee , struct tableau *descente){
	int l = log(descente->size)/log(2)-1 ; // l=2
    int max = pow(2, l+1); // 7
    #pragma omp parallel for
    for (int i = pow(2, l); i <max; i++) {// 3 - 6 //
        descente->current[i] = descente->current[i]+ montee->current[i];
    }

}

// Parallel
void  prefixSum(struct tableau * firsttable,struct tableau *finalresult){
	struct tableau * climbing = create_tableau(((firsttable->size)*2) );
	struct tableau * descent = create_tableau(((firsttable->size)*2) );

	montee(firsttable,climbing);
	descente(climbing,descent);
	finale(climbing,descent);

	#pragma omp parallel for
	for (int i= 0;i<firsttable->size;i++) 
		{
			finalresult->current[i]=descent->current[i+(firsttable->size)];
			//printf("%lld\n", finalresult->current[i]);
		}
	free(climbing->current);
	free(climbing);
	free(descent->current);
	free(descent);
}


/*END PREFIX SUM*/


/*SUFFIX SUM*/

void copy_tableau(struct tableau * firsttable, struct tableau * copie){
	//#pragma omp parallel for
	for (int i=0; i< firsttable->size; i++){
   		copie->current[i]= firsttable->current[i];
   	} 
}

void inverse_tableau(struct tableau * tableauAInverser, struct tableau * tableauInverse){
	copy_tableau(tableauAInverser,tableauInverse);
	for (int i=0 ; i< tableauAInverser->size; i++){
		tableauInverse->current[i]=tableauAInverser->current[(tableauInverse->size)-i-1];
	}
}

void suffixSum(struct tableau * firsttable,struct tableau *finalresult){
	struct tableau * inverse = create_tableau(firsttable->size);
	struct tableau * intermediate =create_tableau(firsttable->size);

	inverse_tableau(firsttable,inverse);
	prefixSum(inverse,intermediate);
	inverse_tableau(intermediate,finalresult);
	
	free(inverse->current);
	free(inverse);
	free(intermediate->current);
	free(intermediate);
}
/* END SUFFIX SUM*/

/*PREFIX MAX*/
void montee_max(struct tableau *firsttable , struct tableau *climbing)
{
	
	for (int i=0 ; i< firsttable->size ;i++){ 
		climbing->current[i+(firsttable->size)] = firsttable->current[i];
	}
	for (int l=(log(firsttable->size)/log(2))-1;l >= 0; l--){ 
		int max=pow(2,l+1);
		#pragma omp parallel for

		for(int j= pow(2,l); j<max;j++){ 
			climbing->current[j]= maximum(climbing->current[(2*j)], climbing->current[(2*j)+1]);
		}
	}


}

void descente_max(struct tableau *climbing , struct tableau *descent){
	descent->current[1]=0; // car racine 

	for (int l= 1; l<(log(climbing->size)/log(2));l++){
		int max=pow(2,l+1); 
		#pragma omp parallel for
		for (int j = pow(2,l); j < max; j++) 
		{
			if ((j)%2 == 0) 	//even
			{
                descent->current[j] =descent->current[j/2] ;
            }
            else { 			//odd
            	descent->current[j] = maximum(descent->current[(j)/2],climbing->current[j-1]);
            }
		}
	}

}


void finale_max(struct tableau *montee , struct tableau *descente){
	int l = log(descente->size)/log(2)-1 ; // l=2
    int max = pow(2, l+1); // 7
    
    #pragma omp parallel for
    for (int i = pow(2, l); i < max; i++) {// 3 - 6 //
        descente->current[i] = maximum(descente->current[i], montee->current[i]);
    }
}
void  prefixMax(struct tableau * firsttable,struct tableau *finalresult){

	struct tableau * climbing = create_tableau(((firsttable->size)*2) );
	struct tableau * descent = create_tableau(((firsttable->size)*2) );

	montee_max(firsttable,climbing);
	descente_max(climbing,descent);
	finale_max(climbing,descent);
	#pragma omp parallel for
	for (int i= 0;i<firsttable->size;i++) 
		{
			finalresult->current[i]=descent->current[i+(firsttable->size)];
		}
	free(climbing->current);
	free(climbing);
	free(descent->current);
	free(descent);
}
/*END PREFIX MAX*/

/*SUFFIX MAX*/


void suffixMax(struct tableau * firsttable,struct tableau *finalresult){
	struct tableau * inverse = create_tableau(firsttable->size);
	struct tableau * intermediate =create_tableau(firsttable->size);

	inverse_tableau(firsttable,inverse);
	prefixMax(inverse,intermediate);
	inverse_tableau(intermediate,finalresult);

	free(inverse->current);
	free(inverse);
	free(intermediate->current);
	free(intermediate);
}
/*END SUFFIX MAX*/


/* FINAL SUM */

//Ms[i] = PMAX[i] - SSUM[i] + Q[i]
//Mp[i] = SMAX[i] - PSUM[i] + Q[i]
//M[i] = Ms[i] + Mp[i] - Q[i]
struct tableau * finaleSum(struct tableau *M,struct tableau * psum,struct tableau * pmax,struct tableau * ssum,struct tableau * smax, struct tableau * q){
	//#pragma omp parallel for
	int index =0;
	long long max =pmax->current[0]-ssum->current[0]+q->current[0]+smax->current[0]-psum->current[0];
	for (int i= 0;i<q->size;i++) //O à 4
		{
			M->current[i]=pmax->current[i]-ssum->current[i]+q->current[i]+smax->current[i]-psum->current[i];
			//printf("%lld\n", finalresult->current[i]);
			if (M->current[i] >max)
			{	
				max= M->current[i];
				index= i;
			}
		}

	// compteur initialisé à un car on a déjà trouvé un max avant
	bool found= false;
	int counter =1;
	for (int i=0;i< q->size;i++){// commencer par i= index pour éviter de boucler sur une partie dans laquelle on sait qu'on a pas de max
			if ( max == M->current[i] ){
				counter=counter+1;
				found= true;
			}
			if ( found ==true && max != M->current[i]){ // if added i should start from index
				break;
			}
			// pour voir si cette condition on peut tester directement sur la méthode 


	}
	struct tableau * finalresult = create_tableau(counter);

	finalresult->current[0]=max;

	//#pragma omp parallel for
	for (int i=1; i<counter ;i++){
		finalresult->current[i]= q->current[i+index-1];
		
	}
	return finalresult;
	

}

/*END FINAL SUM*/

// pour la lecture du fichier :
// 1. https://openclassrooms.com/forum/sujet/extraire-les-donnees-d-un-fichier-stocker-dans-tableau-20104
// 2. https://stackoverflow.com/questions/34745278/how-to-count-the-number-of-integers-in-a-file-in-c
// pour le calcul du temps: 
// 1. https://koor.fr/C/ctime/clock_t.wp

int main(int argc, char *argv[]){
	//clock_t begin = clock();
/*
	int n=0;
	FILE *fichier = NULL;
    fichier = fopen("test1", "r"); // Ouverture du fichier
 
    if (fichier != NULL)
    {
        printf("L'ouverture du fichier a reussie !\n");
 
    while(fscanf(fichier,"%lld") != 1) // On parcours une première fois le fichier pour savoir combien il a de valeurs
        n++;
 
    printf("Le fichier contient %d valeurs.\n", n);
    struct tableau *Q = create_tableau(n);
    if (Q == NULL)
    {
        printf("Erreur allocation !");
        exit(0);
    }
    else
    {
        printf("Allocation reussie !\n");
    }
 
    rewind(fichier);*/
 
 

	//struct tableau * Q = malloc(sizeof(struct tableau ));
	/*Q->size = 16; 
    Q->current = malloc(Q->size*sizeof(long long)); 
    
    
    FILE* fichier = NULL;
    char chaine[TAILLE_MAX] = "";
    fichier = fopen("test1", "r");
    if (fichier != NULL)
    {
        while (fgets(chaine, TAILLE_MAX, fichier) != NULL) // On lit le fichier tant qu'on ne reçoit pas d'erreur (NULL)
        {
            printf("%s", chaine); // On affiche la chaîne qu'on vient de lire
        }
        fclose(fichier);
    }
 	else
 	{
 		printf("impo de trouver le fichier");
	 }*/

    //testing prefix sum

    struct tableau * PSUM= create_tableau(Q->size);
    prefixSum(Q,PSUM);
    //testing suffix sum
    struct tableau * SSUM= create_tableau(Q->size);
    suffixSum(Q,SSUM);
    /*for (int i=0; i< PSUM->size; i++){
		printf("%lld ",PSUM->current[i] );
		
	}
	printf("\n");
	for (int i=0; i< SSUM->size; i++){
		printf("%lld ",SSUM->current[i] );
		
	}*/
    //testing suffix max
    struct tableau * SMAX= create_tableau(Q->size);
    suffixMax(PSUM,SMAX);

    //testing prefix max
    struct tableau * PMAX= create_tableau(Q->size);
    prefixMax(SSUM,PMAX);

    /*for (int i=0; i< SMAX->size; i++){
		printf("%lld ",SMAX->current[i] );
		
	}
	printf("\n");
	for (int i=0; i< PMAX->size; i++){
		printf("%lld ",PMAX->current[i] );
		
	}*/

    
    //testing final result
    struct tableau * M= create_tableau(Q->size);

	struct tableau * tabb;
	tabb =finaleSum(M,PSUM,PMAX,SSUM,SMAX, Q);
	for (int i=0; i< tabb->size; i++){
		printf("%lld ",tabb->current[i] );
		
	}
	printf("\n");
	free(SSUM->current);
	free(SSUM);
	free(PSUM->current);
	free(PSUM);
	free(SMAX->current);
	free(SMAX);
	free(PMAX->current);
	free(PMAX);
	free(Q->current);
	free(Q);
	free(M->current);
	free(M);
	free(tabb->current);
	free(tabb);
	//clock_t end = clock();
	//printf("Elapsed: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
	return 0;

}

