
/*
        Title: Graph Input/Output Source File.
        file: graph.c
        does: graph input and output in DIMACS standard to
		file (either ASCII or BINARY), modified to 
		Also provides graph structure access etc.
        Source: Joseph Culberson's Coloring Page
                http://web.cs.ualberta.ca/~joe/Coloring/index.html
        Author: Original code by Tamas Badics
                using the technique of Marcus Peinado, Boston University
                Modified to be integrated with the graph coloring program by:
                        Joseph Culberson, Adam Beacham and Denis Papp.

        email: joe@cs.ualberta.ca

	Modifications Copyright (c) 1997 Joseph Culberson. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions of source code must retain the above copyright
   	notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
   	notice, this list of conditions and the following disclaimer in the
   	documentation and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this 
	software must display the following acknowledgement:
     	This product includes software developed by J. Culberson at the
     	University of Alberta, Edmonton.
	4. Neither the name of the University nor the names of its contributors
   	may be used to endorse or promote products derived from this software
   	without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	THIS SOFTWARE IS SUPPLIED WITHOUT ANY SUPPORT SERVICES. 

*/

#include "mysys.h"
#include <ctype.h>
#include "graph.h"

#define DEBUGVV 1
#define DEBUG 1
/*#define DEBUGPRINT */ 

/* Global variables */
/* GRAPH */
adjacencytype graph[GRAPHSIZE];
vertextype order;

/* partition results for partite graphs  for purity measure */
int partset[MAXVERTEX];
int partitionflag;
int partitionnumber;


void read_graph_DIMACS_bin(char *);

#ifdef DEBUGPRINT
void printgraph(adjacencytype graph[],int);
#endif

void read_graph_DIMACS_ascii(char *file)
{
        int c, oc;
        int i,j,numedges,edgecnt;
	char tmp[80];
        FILE *fp;

	bzero(graph,GRAPHSIZE);

        if ( (fp=fopen(file,"r"))==NULL )
          { printf("ERROR: Cannot open infile\n"); exit(10); }

        for(oc = '\0' ;(c = fgetc(fp)) != EOF && 
	     ((oc != '\0' && oc != '\n') || c != 'p')
                ; oc = c);

	if (!fscanf(fp, "%s %d %d\n", tmp, &order, &numedges)) {
		printf("ERROR: corrupted inputfile in p\n");
		exit(10);
	}
	printf("number of vertices = %d\n",order);

	/* read until hit 'e' lines or a 'c' line specifying the
	   presence of a cheat */

        for(oc='\n' ;(c = fgetc(fp)) != EOF && 
			(oc != '\n' || c != 'e'); oc = c) {
		switch (c) {
		  case 'c':
			if (oc=='\n') {
				fscanf(fp,"%s ",tmp);
			}
			break;
		  default:
			break;
		}
	}

        ungetc(c, fp);

	edgecnt=0;
        while ((c = fgetc(fp)) != EOF){
                switch (c) {
                  case 'e':
                  	if (!fscanf(fp, "%d %d", &i, &j)) { 
				printf("ERROR: corrupted inputfile\n");
				exit(10); 
			}

			edgecnt++;
			i--; j--;
                        setedge((i),(j));
                        setedge((j),(i));
                        break;

                  case '\n':
                  default:
                        break;
                }
        }

        fclose(fp);
	printf("p edge %d %d\n",order,numedges);
	printf("Number of edges = %d edges read = %d\n",numedges,edgecnt);
}

/* 
 * getgraph()
 * GIVEN: filename
 * DESC: reads the graph into the global variables graph and order
 */
void getgraph(char *file)
{
	int format;
	FILE *fp;

	partitionflag=0;

	fp = fopen(file,"r");	
	if (fp==NULL) {
		fprintf(stderr,"Bad file name.\n");
		exit(1);
	}
	/* read first byte of file to guess at format */
	format = fgetc(fp);
	fclose(fp);


	if (isdigit(format)) {
		printf("Binary format\n");
		read_graph_DIMACS_bin(file);
	} else {
		printf("ASCII format\n");
		read_graph_DIMACS_ascii(file);
	}
	
#ifdef DEBUGPRINT
	printgraph(graph,order);
#endif
}


/*
 * invertbyte()
 * GIVEN: a pointer to an 8-bit byte
 * DESC: inverts the bits (reverse order)
 */
void invertbyte(unsigned char *byte)
{
	int i;
	int anew = 0;

	for (i=0;i<8;i++) 
		if (*byte & (1 << (7-i))) 
			anew |= (1 << i);

	*byte = anew;
}

void read_graph_DIMACS_bin( char *file)
{
	int c,oc;
	int i,j, length = 0;
	char tmp[80];
        int numedges;
	FILE *fp;

	if ( (fp=fopen(file,"r"))==NULL )
	  { printf("ERROR: Cannot open infile\n"); exit(10); }

	if (!fscanf(fp, "%d\n", &length))
	  { printf("ERROR: Corrupted preamble.\n"); exit(10); }

	/* Dont need to know the preambles length
	   if(length >= MAX_PREAMBLE)
	  { printf("ERROR: Too int preamble.\n"); exit(10); }
         */

	bzero(graph,GRAPHSIZE);
        
	for(oc = '\0' ;(c = fgetc(fp)) != EOF && 
	     ((oc != '\0' && oc != '\n') || c != 'p')
                ; oc = c);

	if (!fscanf(fp, "%s %d %d\n", tmp, &order, &numedges)) {
		printf("ERROR: corrupted inputfile in p\n");
		exit(10);
	}
	printf("number of vertices = %d\n",order);

	/* read until hit a \n not followed by a 'c' */
        
	for(oc='\n' ;(c = fgetc(fp)) != EOF && 
			(oc != '\n' || c == 'c'); oc = c) {
		if (oc=='\n' && c=='c') {
			fscanf(fp,"%s ",tmp);
		}
	}
        
	ungetc(c, fp);

	for ( i = 0
	   ; i < order && fread(graph+(i*ROWSIZE), 1, (int)((i + 8)/8), fp)
	   ; i++ ) {
		/* invert all the bytes read in */
		for (j=0; j < (int)((i+8)/8); j++) 
			invertbyte(graph+(i*ROWSIZE)+j);
	}	

	/* conversion */
	for (i=0;i<order;i++)
	for (j=0;j<=i;j++) 
	if (!edge(i,j)) 
		clearedge(j,i);
	else 
		setedge(j,i);

	fclose(fp);
	printf("p edge %d %d\n",order,numedges);
}

#ifdef DEBUGPRINT
void printgraph(graph,order)
adjacencytype graph[GRAPHSIZE];
int order;
{
	int i,j;
	printf("GRAPH\n");
	printf("     ");
	/*for (i=0;i<order;i++) printf("%3d",i);*/
	printf("\n");
	for (i=0;i<order;i++) {
		printf("%3d: ",i);
		for (j=0;j<order;j++) {
			/*if ( edge(i,j) ) printf("  1");
			else printf("  0");*/
			if (j % 8 == 0) printf(" ");
			if (edge(i,j)) printf("1");
			else printf("0");
		}
		printf("\n");
	}
}
#endif

