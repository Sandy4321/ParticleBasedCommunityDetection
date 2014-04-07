/*
 * particle.c
 * Author: apetkova
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "graph.h"
#include "particle.h"

void initParticles(num_particles)
{
    int i;
	//TODO: check whether constant arithmetic is compiler-dependent. Is this going to break?
	double init_omega = (double)OMEGA_MIN + (double)(OMEGA_MAX - OMEGA_MIN)/num_particles;
    
	particles = (particle_type*)malloc(num_particles * sizeof(particle_type));
    for(i=0; i < num_particles; i++)
    {
        particles[i].id = i; 
	    particles[i].energy = init_omega;
        particles[i].state = ACTIVE;			// active == 1, otherwise exhausted
        particles[i].curr_node_id = 0; 
        particles[i].prev_node_id = 0; 
        
    }

}

/* 
 ************************************************
 * Disperese all particles randomly in the network
 */
void initPlaceParticles(num_particles)
{
    /*particles[0].curr_node_id = 5; 
    graph->nodes[5].idx_node.dominator = 0; 
	particles[1].curr_node_id = 25;
	graph->nodes[25].idx_node.dominator = 1;
	*/

	int i, j;
	srand((unsigned)time(NULL));
    for(i=0; i<num_particles; i++)
    {
		j = (rand() % (graph->nvert-1) + 0); 
        particles[i].curr_node_id = j;
        graph->nodes[j].idx_node.dominator = i;
    }

}

/*
 **********************************
 * Print out the particle profiles
 */
void printParticles(num_particles)
{
    int i; 

    for(i=0; i<num_particles; i++)
    {
        printf("\n particle[%d]: " , i); 
        printf("\n\t energy: %g" , particles[i].energy); 
        printf("\n\t state: %d" , particles[i].state); 
        printf("\n\t curr_node_id: %d" , particles[i].curr_node_id); 
        printf("\n\t prev_node_id: %d" , particles[i].prev_node_id); 
    }       
    printf("\n");

}
