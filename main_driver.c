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
	int num_iterations;
	int num_particles;
	double lambda = 0.0;

	if(argc < 5 )	{	// there should be at least 2 arguments and at most 3
		fprintf(stderr, "usage: %s filename num_iterations num_particles lambda labels (optional)\n", argv[0]);
		exit(1);
	} 
	
	//Create the graph from inout file
	readGraph(argv[1]);
	printGraph();	

	//Read in the num_iterations argument from command line
	num_iterations = atol(argv[2]);
	num_particles = atol(argv[3]);
	lambda = atol(argv[4]);

	if(argc == 6)
	    readLabels(argv[5]);

	//Initialize the attributes of each particle
    initParticles(num_particles);

    // TODO: implement the random disperse later. For now, just hardcode \
	 the initial position of the particles
    initPlaceParticles(num_particles);  
    printParticles(num_particles); 

	//Run community detection alg.
	runDetection(num_iterations, num_particles, lambda);	
   

	printGraph();	
    printParticles(num_particles); 
    printClusters(num_particles);
	
	//NOTE: For debugging purposes - follows the avg convergence rate of the \
	alg. on the Zachary Karate Club net
	//printConvergenceZachary();
	
//	destroyGraph();
	return 0;

}
