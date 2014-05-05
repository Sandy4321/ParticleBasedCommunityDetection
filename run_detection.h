/*
 * run_detection.h
 *Author: apetkova
 *
 */
  
#ifndef RUN_DETECTION_H_
#define RUN_DETECTION_H_

#define EPSILON	0.00001
#include "particle.h"

//Drives the community detection algorithm
void runDetection(int num_particles, double alpha, double delta, double epsilon, int weighted_rean);

//void particleRandomWalk(particle_type *p);
void particleRean(particle_type *p, int weighted_rean);
double *particlePrefWalk(particle_type *p);
int selectNextNode(particle_type *p, double *tran_vector, int num_neighbors);

//Calculates the energy level of a given node
void calcEnergy(particle_type *p);

// Calculates the new dominator (particle) of the newly occupied, by particle j, node i
void compNodeDominator(particle_type *p, int num_particles);
double compAvgMaxFrequency(int num_particles);
//Computes N bar ( the relative frequency of visits of particle k \
to node i) for particle k on node i
//double compNBar(int k, int i);

//Computes the infinity norm of ( NBar(t)-NBar(t-1)) in order to \
compare to EPSILON to approximate a fixed point
double compFixPoint(int num_particles);

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

//Computes the avg number of particle visitors per node
int avgNumParticlesPerNode(int num_particles);

// Print the final clusters 
void printClusters(int num_particles, int label_flag);

void initCountN(int rows, int cols);
void syncCountN(int rows);
void incrementMatrixN(int i, int j);
double getNBar(int i, int j);
double getNBarPrev(int i, int j);
double getNBarPrevParticle(int i, int j);

void createMatrixN(int rows, int cols);
void deleteMatrixN();
void addMiniParticle(int i, int k);
void printMatrixN(int rows);
mini_particle_type* getMatrixN(int i, int j);
double getMatrixNCount(int i, int j);
void compareMatrixN(); 



#endif  /*  _RUN_DETECTION_H_ */
