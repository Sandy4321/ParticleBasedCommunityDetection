/*
 * Contains main().
 * Author: apetkova
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "main_driver.h"
#include "graph.h"
#include "run_detection.h"

int main(int argc, char **argv)
{
	// Variable declaration
	int i,j=1;

	if(argc < 2 )	{	// there should be at least 2 arguments and at most 3
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(1);
	} 
	
	//Create the graph from inout file
	readGraph(argv[1]);
	printGraph();	
	
	//Initialize the attributes of each particle
    initParticles();

    // TODO: implement the random disperse later. For now, just hardcode \
	 the initial position of the particles
    initPlaceParticles();  
    //printParticles(); 

	//Run community detection alg.
	runDetection();	

	printGraph();	
    //printParticles(); 
    printClusters();
	
	destroyGraph();
	return 0;
}
