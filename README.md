# K-Coloring-Sat-Solver

The project contains two folders: 
  1. Random Graph Generator
  
Random Graph Generator.
This random.c contains the code to generate a random graph. This takes two inputs.
One being the number of vertices the graph will have and the other input is probability that will determine connecting 
two vertices with an edge.

To run type: 

  gcc random.c
  a.out #n #p > outputFilename.txt
  
The outputFilename.txt will have the graph instance in the following format:

p edge #n #p
e v1 v2
...
...
...



  2. Graph to CNF Converter
  
This folder contains code for reducing a given graph to CNF formula for soving k-coloring problem.
Installation: 
	Run 'make install in the folder form linux.'
  
To run the reducer:
	clr2sat #file #numcolors

where #file will be the name of the file having the graph instance and the #nucolors is the number of colors to be checked.
