/*
 * particle.h
 * Author: apetkova
 *-
 */

#ifndef PARTICLE_H_
#define PARTICLE_H_

#define NUM_PARTICLES 2
#define OMEGA_MAX 1
#define OMEGA_MIN 0
#define DELTA 0.1
#define ALPHA 0.4
//pertaining to particle state \
counterintuitive values but match the paper \
and necessary so for the transition matrix
#define ACTIVE 0		
#define EXHAUSTED 1


#include "graph.h"

// particle struct
typedef struct particle
{
    int id;
    double energy;
    int state;
    int curr_node_id;
    int prev_node_id;
} particle_type;

particle_type *particles;

// Allocate memory for particles
void initParticles();

// Disperse the particles at random on the graph.
void initPlaceParticles(); 

#endif
