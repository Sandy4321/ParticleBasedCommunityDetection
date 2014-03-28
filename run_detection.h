/*
 * run_detection.h
 *Author: apetkova
 *
 */
  
#ifndef RUN_DETECTION_H_
#define RUN_DETECTION_H_

#define NUM_ITERATIONS 1000

#include "particle.h"

//Drives the community detection algorithm
void runDetection();

void particleRandomWalk(particle_type *p);
void particleRean(particle_type *p);

void initMatrixN(int rows, int cols);

//Creates a multidimensional data structure according to passed size parameters
double ** create2DArray( int row, int col );


#endif  /*  _RUN_DETECTION_H_ */
