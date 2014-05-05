/*
 * Contains main().
 * Author: apetkova
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "main_driver.h"
#include "graph.h"
#include "run_detection.h"

int main(int argc, char **argv)
{
	// Variable declaration
	int i=0,j=1;
	int weighted_rean = 0;
	int c = 0;
	int num_particles = 0;
	double delta = 0.0;
	double epsilon = 0.0;
	double lambda = 0.0;
	int label_flag = 0;
	char *label_filename = NULL;
	char *input_filename = NULL;

	while ((c = getopt (argc, argv, "wl:")) != -1)
         switch (c)
         {
           case 'w':
             weighted_rean = 1;
             break;
           case 'l':
             label_filename = optarg;
			 label_flag = 1;
             break;
           case '?':
             if (optopt == 'c')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             else
               fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
             return 1;
           default: fprintf(stderr, "Usage: %s [-w] [l labels.filename] input_file no_particles lambda delta epsilon \n", argv[0]);				
             abort ();
         }


	if((argc-optind+1) < 5 )	{	// there should be at least 2 arguments and at most 3
		fprintf(stderr, "Usage: %s [-w] [l labels.filename] input_file no_particles lambda delta epsilon \n", argv[0]);
		exit(1);
	} 
	else{	
		//Create the graph from inout file
		input_filename = argv[optind];

		//Read in the num_iterations argument from command line
		//num_iterations = atoi(argv[2]);
		num_particles = atoi(argv[optind+1]);
		lambda = atof(argv[optind+2]);
		delta = atof(argv[optind+3]);
		epsilon = atof(argv[optind+4]);
	}
	
	//Create the graph from inout file
    readGraph(input_filename);
    if(label_filename!= NULL)
		readLabels(label_filename);

	printGraph();	
	
	//Initialize the attributes of each particle
    initParticles(num_particles);

    // TODO: implement the random disperse later. For now, just hardcode \
	 the initial position of the particles
    initPlaceParticles(num_particles);  
    printParticles(num_particles); 

	//Run community detection alg.
	runDetection(num_particles, lambda, delta, epsilon, weighted_rean);	
   

	printGraph();	
    printParticles(num_particles); 
    printClusters(num_particles, label_flag);
	
	//NOTE: For debugging purposes - follows the avg convergence rate of the \
	alg. on the Zachary Karate Club net
//	printConvergenceZachary();
	
	destroyGraph();
	return 0;

}
