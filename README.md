# K-Coloring-Sat-Solver

The project contains two folders: 
  1. Random Graph Generator
  
Random Graph Generator.
This random.c contains the code to generate a random graph. This takes two inputs.
One being the number of vertices the graph will have and the other input is probability that will determine connecting 
two vertices with an edge.

To run type: 

  gcc random.c <br />  
  a.out #n #p > outputFilename.txt
  
The outputFilename.txt will have the graph instance in the following format:

p edge #n #p<br />
e v1 v2<br />
...<br />
...<br />
...



  2. Graph to CNF Converter
  
This folder contains code for reducing a given graph to CNF formula for soving k-coloring problem.<br />
Installation: <br />
	Run 'make install' in the folder in linux directory.
  
To run the reducer:<br />
	'clr2sat #file #numcolors'

where #file will be the name of the file having the graph instance and the #numcolors is the number of colors to be checked.
