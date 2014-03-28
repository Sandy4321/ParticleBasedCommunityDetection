/*
 * run_detection.c
 * Author: apetkova
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "graph.h"
#include "particle.h"

void initParticles()
{
    int i;
	//TODO: check wether constant arithmetic is compiler-dependent. Is this going to break?
	double init_omega = (double)OMEGA_MIN + (double)(OMEGA_MAX - OMEGA_MIN)/NUM_PARTICLES;
    
	particles = (particle_type*)malloc(NUM_PARTICLES * sizeof(particle_type));
    for(i=0; i<NUM_PARTICLES; i++)
    {
        particles[i].id = i; 
	    particles[i].energy = init_omega;
        particles[i].state = ACTIVE;			// active == 1, otherwise exhausted
        particles[i].curr_node_id = 0; 
        particles[i].prev_node_id = 0; 
        
    }

}

// TODO: implement the random disperse later. For now, just harcode the starting location of the particles
void initPlaceParticles()
{
    int i; 

    for(i=0; i<NUM_PARTICLES; i++)
    {
        particles[i].curr_node_id = i; 
        graph->neighbors[i].idx_node.dominator = i; 
    }       
}

void printParticles()
{
    int i; 

    for(i=0; i<NUM_PARTICLES; i++)
    {
        printf("\n particle[%d]: " , i); 
        printf("\n\t energy: %g" , particles[i].energy); 
        printf("\n\t state: %d" , particles[i].state); 
        printf("\n\t curr_node_id: %d" , particles[i].curr_node_id); 
        printf("\n\t prev_node_id: %d" , particles[i].prev_node_id); 
    }       
    printf("\n");

}
