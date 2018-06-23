#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int tvertices;
double prob;

int bernoulli_distributed(double p){
    return (p > ((double)rand())/RAND_MAX);
}

int main(int argc, char** argv) {

    	if( argc < 3 ){
		printf("Error...!!\n");
      exit(1);
	}
	else {
		tvertices = atoi(argv[1]);
		prob = atof (argv[2]);
                printf("p edge %d %f", tvertices, prob);
                for(int i=0;i<tvertices;i++)
                {
                    for(int j=i+1;j<tvertices;j++){
                        if (bernoulli_distributed(prob)){
                            printf ("e %d %d \n", i, j);
                            
                        }
                        }
                            
                     
                    }
                }
	}