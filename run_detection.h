/*
 * run_detection.h
 *Author: apetkova
 *
 */
  
#ifndef RUN_DETECTION_H_
#define RUN_DETECTION_H_

#define NUM_ITERATIONS 1000
#define EPSILON	0.05
#include "particle.h"

//Drives the community detection algorithm
void runDetection();

//void particleRandomWalk(particle_type *p);
void particleRean(particle_type *p);
double *particlePrefWalk(particle_type *p);
void selectNextNode(particle_type *p, double *tran_vector, int num_neighbors);

//Calculates the energy level of a given node
void calcEnergy(particle_type *p);

// Calculates the new dominator (particle) of the newly occupied, by particle j, node i
void compNodeDominator(particle_type *p);

//Computes N bar ( the relative frequency of visits of particle k \
to node i) for particle k on node i
double compNBar(int k, int i);

//Computes matrixNBar containing the relative frequencies of \
visits of particle k to node i
void compMatrixNBar();

//Computes the infinity norm of ( NBar(t)-NBar(t-1)) in order to \
compare to EPSILON to approximate a fixed point
double compFixPoint();

//Initializes matrixN containing the freaquencies of visits of \
 of particle k to node i
void initMatrixN(int rows, int cols);

//Creates a multidimensional data structure according to passed size parameters
double ** create2DArray( int row, int col );

//Update energy out an array
void print2DArr(double **arr, int rows, int cols);

//Finds max value in an array of floats and returns its INDEX. It assumes that there is at least 1 \
non-negative element in the array.
int max_array(double *arr, int arrLen);

#endif  /*  _RUN_DETECTION_H_ */
