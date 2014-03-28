/*
 * run_detection.c
 * Author: apetkova
 *
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "run_detection.h"
#include "main_driver.h"

//matrix N, keeping track of the number of visits \
by all particles to every node. dim(N) = |V|xK	
double **matrixN;

/*
 *
 * Drives the community detection algorithm
 */
void runDetection()
{

    int i,j;
	
    assert(graph != NULL);
    srand(time(NULL));

	matrixN = create2DArray(graph->nvert, NUM_PARTICLES);
	j = (matrixN == NULL);
	CHECK(j, "Unable to allocate memroy for matrixN");
	
	//Initialize matrix N, where (i, j) = num of times particle j \
	  visited node i
	initMatrixN(graph->nvert, NUM_PARTICLES);

	// Main descrete- event driving loop. 
	// In each iteration, each particle performs a walk
    for(i=0; i<NUM_ITERATIONS; i++)
    {
        for(j=0; j<NUM_PARTICLES; j++)
        {
            if(particles[j].state == ACTIVE) 
                particleRandomWalk(&particles[j]);
            else{
				// if particle's energy is drained - reanimate
                particleRean(&particles[j]);
			}
         
        }
        //printf("\n\n Iteration %d\n", i);
        //printParticles();
    }

	//calcPrand();


}

/*
 * Reanimates a particle. 
 * Selects a random (weighted probability) node among the ones dominated by the particle, 
 * and moves the particle there. 
 * If there are no nodes dominated by this particle, one is selected at random. */ 
void particleRean(particle_type *p)
{
    int i; 
    int total_visits = 0; 
    int num_dominated_nodes = 0; 

    // count dominated notes and total number of visits to them
    for (i = 0; i < graph->nvert; i++)
    {
        if(p->id == graph->neighbors[i].idx_node.dominator)
        {
            total_visits = total_visits + matrixN[i][p->id]; 
            num_dominated_nodes++; 
        }
    }

    // perform weighted random selection of dominated node
    if(num_dominated_nodes > 0)
    {
        int weighted_prob = rand()%total_visits + 1; 
        int step = 0; 
        for(i=0; i<graph->nvert; i++)
        {
            if(p->id == graph->neighbors[i].idx_node.dominator)
            {
                step = step + matrixN[i][p->id]; 
                if(step >= weighted_prob)
                {
                    p->prev_node_id = p->curr_node_id; 
                    p->curr_node_id = i; 
                    p->energy = p->energy + DELTA; 
                    p->state = ACTIVE; 
                    matrixN[i][p->id]++;
                }
            }
        }    
    }
    else
    {
        int prob = rand()%(graph->nvert);
        p->prev_node_id = p->curr_node_id;
        p->curr_node_id = prob; 
        p->state = ACTIVE;
        matrixN[prob][p->id]++;
    }
}

// Pick a random number between 1 - num_neighbors, and then walk the list to that neighbor
//
// TODO: Make sure that you are not going back to the node that you just came from (unless you have no other choice) 
void particleRandomWalk(particle_type *p)
{
    int i; 
    adjlist_node_type *listPtr = graph->neighbors[p->curr_node_id].head;

    assert(NULL != listPtr);
    
    int num_neighbors = graph->neighbors[p->curr_node_id].num_neighbors;
    int neighbor_number = rand() % num_neighbors;

    for(i=0; i<neighbor_number; i++)
    {
        listPtr = listPtr->next;     
        assert(NULL != listPtr); 
    }
    
    p->prev_node_id = p->curr_node_id;
    p->curr_node_id = listPtr->id; 

    // Update energy
    int dominator = graph->neighbors[p->curr_node_id].idx_node.dominator;
    if(p->id == dominator)
        p->energy = p->energy + DELTA; 
    else
        p->energy =  p->energy - DELTA;
    
    if(p->energy > OMEGA_MAX)
    {
        p->energy = OMEGA_MAX; 
    }

    // Check if particle is exhausted 
    if(p->energy < OMEGA_MIN)
    {
        p->energy = OMEGA_MIN; 
        p->state = EXHAUSTED; 
    }

    // Update matrixN
    matrixN[p->curr_node_id][p->id]++; 

    // Recalculate the new dcminator for that node
    int max_visits = 1; 
    int new_dominator = dominator; 
    for(i=0; i<NUM_PARTICLES; i++)
    {
        int temp; 
        temp = matrixN[p->curr_node_id][i]; 
        if(temp > max_visits)
        {
            max_visits = temp; 
            new_dominator = i; 
        }
    }
    graph->neighbors[p->curr_node_id].idx_node.dominator = new_dominator; 

}

/*
 *************************************
 * Initializes matrix N with 1 if the particle k did not 
 * originate (during initializatin) at node i, and with 2, otherwise 
 */
void initMatrixN(int rows, int cols)
{
	int i, k;

	for(i=0; i<rows; i++){
		printf("%d->\t", i);
		for(k=0; k<cols; k++){
			if(particles[k].curr_node_id == i)
				matrixN[i][k] = 2.0;
			else
				matrixN[i][k] = 1.0;
			printf("%g\t", matrixN[i][k]);
		}
		printf("\n*************\n");
	}
}

//TODO Create function printMatrixN


/*
 *************************************
Creates a multidimensional data structure according to passed size parameters
*/
double ** create2DArray( int row, int col ) {
    double ** array_ptr;
    int i;

    array_ptr = malloc( sizeof( double *) * row );

    array_ptr[0] = (double *)malloc(row * col * sizeof(double));
    for(i = 1; i < row; i++){
		array_ptr[i] = array_ptr[0] + i * col;
    }

    //Check if memory allocation was successful
    if(array_ptr == NULL)
    {
		printf("Out of memory. Could not create an array.\n");
		return NULL;
    }

    return array_ptr;
}
