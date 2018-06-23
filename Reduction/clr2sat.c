

#include "mysys.h"
#include "graph.h"

#define TRUE 1
#define FALSE 0

#define MAXLITERALS 100000 // must include 0's to terminate clauses.

int contype = 0;  // default is standard; 1 for logarithmic form
int addunique = FALSE; // default is not to add clauses to force 
		       // unique color assignments
int addsymmetric = FALSE; // default is not to add unit clauses forcing
			// fixed colorings on vertices of one edge


char allowed_params[4][25] = {"std","log","unique","break"};

void usage()
{
	printf("\tUsage: clr2sat file num_colors [conversion_type] [extra[extra]]\n");
	printf("\t\tconversiontypes:\tstd (default) or log\n");
	printf("\t\textras: unique, break(to force color one edge)\n");
	printf("\t\tYou may add neither, one or both extras\n");
	exit(1);
}

void parseparams(int argc, char *argv[], int *ncolors)
{
	int i,j;
	if (argc < 3) {
		usage();
	}

	/* Set up graph */
	getgraph(argv[1]);

	*ncolors = atoi(argv[2]);
	if (*ncolors < 2) {
		printf("Illegal number of colors\n");
		usage();
	}

	for(i=3;i<argc;i++) {
		for(j=0;j<5;j++) {
		 if (!strcmp(allowed_params[j],argv[i])) 
			 break;
		}
		if (j==0) {
			contype = 0; //standard
		} else if (j==1) {
			contype = 1; //logarithmic
		} else if (j==2) {
			addunique = TRUE; //add clauses force unique
		} else if (j==3) {
			addsymmetric = TRUE; //add clauses to color one edge
		} else {
			printf("Illegal Parameter --%s--\n",argv[i]);
			usage();
		}

	}
}

int literals[MAXLITERALS];
int numclauses = 0;
int numliterals = 0;
int numvars = 0;

void convert(int ncolors)
{
	int i,j,k,v,w;
	int symflag;

	numvars = order*ncolors;

	// vertices 1 k-clause and (k choose 2) 2-clauses
	for(v=1;v<= order;v++)
	{
		k = ((v-1)*ncolors);

		for(i=1;i<=ncolors;i++)
		{
			literals[numliterals] = k+i;
			numliterals++;
			// fprintf(fp,"%d ",k+i);
		}
		literals[numliterals] = 0; // separator
		numliterals++;
		numclauses++;
		// fprintf(fp,"\n");
	}

	if (addunique) {
	  for(v=1;v<= order;v++)
	  {
		k = ((v-1)*ncolors);
		for ( i = 1; i< ncolors; i++)
		for(j=i+1;j<=ncolors;j++)
		{
			literals[numliterals] = -(k+i);
			numliterals++;
			literals[numliterals] = -(k+j);
			numliterals++;
			literals[numliterals] = 0;
			numliterals++;
			numclauses++;
			// fprintf(fp,"%d %d\n",-(k+i),-(k+j));
		}
	  }
	}

	/* now the edges */
	// k*e 2-clauses
	symflag = 0;
	for(i=0;i<order-1;i++)
	{
		for(j=i+1;j<order;j++)
		{
			if (edge(i,j)) {
				v = i*ncolors;
				w = j*ncolors;
				for(k=1;k<=ncolors;k++)
				{
					literals[numliterals] = -(v+k);
					numliterals++;
					literals[numliterals] = -(w+k);
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					// fprintf(fp,"%d %d\n", -(v+k), -(w+k));
				}
				if ((addsymmetric) && (symflag==0))
				{
					/* add symmetry breaking 
					 * first edge will have colors 1 and 2
					 * forced
					*/
					symflag = 1;
					literals[numliterals] = v+1;
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					literals[numliterals] = w+2;
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					// fprintf(fp,"%d\n",v+1);
					// fprintf(fp,"%d\n",w+2)
				}
			}
		}
	}
}

// The following are for the logarithmic codes version

void negdecode(int v, int c,  int z)
	/*
	 * print the variables corresponding to the negation of color
	 * c on vertex v where there are z per vertex
	*/
{
	int i;

	for(i=0;i<z;i++)
	{
		if (c & (1<<i))
		{
			literals[numliterals] =  -(((v-1)*z)+(i+1));
			numliterals++;
			// fprintf(fp,"%d ", -(((v-1)*z)+(i+1)));
		} else 
		{
			literals[numliterals] = (((v-1)*z)+(i+1));
			numliterals++;
			// fprintf(fp,"%d ", (((v-1)*z)+(i+1)));
		}
	}
}

void convertlog(int ncolors)
{
	/* This version will implement the z=ceil(log_2(k)) reduction
	 * where for each vertex we have z variables representing the set of
	 * colors and the k edge clauses are each of size 2z
	*/


	int i,j,k,v;

	int z,max;

	int symset;

	max = 2; z =1;
	while (max < ncolors) { max *= 2; z++; }

	numvars = order*z;
 
	/* vertices - here I only need to print the clauses 
	 * that correspond to integers that are not colors 
	*/
	for(v=1;v<= order;v++)
	{
		for(j=max-1; j >= ncolors; j--)
		{
			negdecode(v,j,z);
			literals[numliterals] = 0;
			numliterals++;
			numclauses++;
			// fprintf(fp,"\n");
		}
	}

	/* now the edges */
	symset = FALSE;
	for(i=0;i<order-1;i++)
	{
		for(j=i+1;j<order;j++)
		{
			if (edge(i,j))
			{
				for(k=0;k<ncolors;k++)
				{
					negdecode(i+1,k,z);
					negdecode(j+1,k,z);
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					// fprintf(fp,"\n");
				}
				if ((addsymmetric) && (!symset)) {
					symset = TRUE;
					// force i to be color 1
					literals[numliterals] = -(i*z+1);
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					literals[numliterals] = -(i*z+2);
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					literals[numliterals] = j*z+1;
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
					literals[numliterals] = -(j*z+2);
					numliterals++;
					literals[numliterals] = 0;
					numliterals++;
					numclauses++;
				}
			}
		}
	}


}


void genandprint(char *fname, int ncolors){

	int i;

	char outname[100];

	FILE *fp;

	strcpy(outname,fname);
	strcat(outname,".cnf");

	fp = fopen(outname,"w");

	fprintf(fp,"c CNF SAT conversion from Graph Coloring\n");
	fprintf(fp,"c Input File name: %s\n",fname);
	fprintf(fp,"c Number of Colors  %d\n",ncolors);
	fprintf(fp,"c Conversion type = ");
	if (contype == 0) fprintf(fp,"Standard\n"); 
	else if (contype == 1) fprintf(fp,"Logarithmic\n");

	if (addunique)
		fprintf(fp,"c Clauses added to force unique color assignment\n");
	if (addsymmetric)
		fprintf(fp,"c Clauses added to force color one edge to break symmetry\n");

	// following is for mechanized parsing
	fprintf(fp,"c Conversion Code: ");
	if (contype == 0) fprintf(fp,"S");
	else if (contype == 1) {
		fprintf(fp,"L");
		addunique = TRUE; // implied by log conversion
	}
	if (addunique) fprintf(fp,"U");
	else fprintf(fp,"-");
	if (addsymmetric) fprintf(fp,"B");
	else fprintf(fp,"-");
	fprintf(fp,"\n");

	if (contype == 0){
		convert(ncolors);
		fprintf(fp,"p cnf %d %d\n",numvars,numclauses);
	}
	else if (contype == 1){
		convertlog(ncolors);
		fprintf(fp,"p cnf %d %d\n",numvars,numclauses);
	}


	for(i=0;i<numliterals;i++) {
		fprintf(fp,"%d ",literals[i]);
		// in case people want to read it
		if (0 == literals[i]) fprintf(fp,"\n");
	}

	fclose(fp);
}


	
int main(int argc, char *argv[])
{

	int ncolors; 

	parseparams(argc, argv, &ncolors);

	genandprint(argv[1],ncolors);
	return(0);
}

