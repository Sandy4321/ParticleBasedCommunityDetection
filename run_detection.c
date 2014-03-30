/*
 * run_detection.c
 * Author: apetkova
 *
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "run_detection.h"
#include "main_driver.h"

//matrix N, keeping track of the number of visits \
by all particles to every node. dim(N) = |V|xK	
double **matrixN;
double **matrixNBar;
double **matrixNBarCopy;
/*
 **************************************
 * Drives the community detection algorithm
 */
void runDetection()
{

	int i=0, j, k, out;
	int num_neighbors;
	double rand_prob;
	double fixed_pt = 0.0;
	double *pref_prob = NULL;
	double *tran_vector = NULL;
	adjlist_node_type *listPtr = NULL;
	
    assert(graph != NULL);
    srand(time(NULL));

	matrixN = create2DArray(graph->nvert, NUM_PARTICLES);
	out = (matrixN == NULL);
	CHECK(out, "Unable to allocate memroy for matrixN");

	matrixNBar = create2DArray(graph->nvert, NUM_PARTICLES);
    out = (matrixNBar == NULL);
    CHECK(out, "Unable to allocate memroy for matrixNBar");

	matrixNBarCopy = create2DArray(graph->nvert, NUM_PARTICLES);
	out = (matrixNBarCopy == NULL);
	CHECK(out, "Unable to allocate memroy for matrixNBarCopy");
	
	//Initialize matrix N, where (i, j) = num of times particle j \
	  visited node i
	initMatrixN(graph->nvert, NUM_PARTICLES);
	//Compute matrixNBar
	compMatrixNBar();

	// Main descrete-time driving loop. 
	// In each iteration, each particle performs a walk
    do
    {
        for(j=0; j<NUM_PARTICLES; j++)		// for EACH PARTICLE 
        {
            if(particles[j].state == ACTIVE){ 
				//find the num of neighbors of the node particle j \
				currently occupies
				num_neighbors = graph->nodes[particles[j].curr_node_id].num_neighbors;
	//			printf("num_neighbors = %d\n", num_neighbors);
				listPtr = graph->nodes[particles[j].curr_node_id].head;
				assert(NULL != listPtr);
			/*	printf("particle %d\tcurr node = %d\tneighbors:", particles[j].id, particles[j].curr_node_id);
				while(listPtr->next!=NULL){
					printf("->%d", listPtr->id);
					listPtr = listPtr->next;
				}*/
				printf("\n");
				//compute the rand probability to visit a \
				neighbor of nodes[particles[j].curr_node_id]
                rand_prob = 1/(double)num_neighbors;
				//printf("rand_prob = %g\n", rand_prob);			
				//compute the pref walk vector
				pref_prob = particlePrefWalk(&particles[j]);
				
				//compute transition vector
				tran_vector = (double*)malloc(num_neighbors*sizeof(double));
				out = (tran_vector == NULL);
			    CHECK(out, "Unable to allocate memroy for tran_vector");
				for(k=0; k < num_neighbors; k++){
					tran_vector[k] = (1-ALPHA)*rand_prob + ALPHA*pref_prob[k];
				//	printf("%g | ", tran_vector[k]);
				}
			//	printf("\n");
				
				//make a final selection of the node to transition to
				selectNextNode(&particles[j], tran_vector, num_neighbors);

				//update particle energy level after move
				calcEnergy(&particles[j]);
				
				// Update matrixN
                matrixN[particles[j].curr_node_id][particles[j].id]++;
	//			printf("matrixN\n-----------------\n");
    //          print2DArr(matrixN, graph->nvert, NUM_PARTICLES);		
				//calculate the new dominator of the node particle j just moved to
				compNodeDominator(&particles[j]);
				
				//copy matrixNBar into matrixNBarCopy
				memcpy(matrixNBarCopy, matrixNBar, graph->nvert* NUM_PARTICLES*sizeof(double));
			//	printf("matrixNBar\n-----------------\n");
			//	print2DArr(matrixNBar, graph->nvert, NUM_PARTICLES);
			//	printf("matrixNBarCopy\n-----------------\n");
			//	print2DArr(matrixNBarCopy, graph->nvert, NUM_PARTICLES);
			}
            else{
				// if particle's energy is drained - reanimate
                particleRean(&particles[j]);
			}
        }
		compMatrixNBar();
        //printf("matrixNBar\n-----------------\n");
        //print2DArr(matrixNBar, graph->nvert, NUM_PARTICLES);
        //Compute the diff b/n NBar(t)-NBar(t-1) in order to \
        compare to EPSILON
        fixed_pt = compFixPoint();
        printf("fixed_pt = %g\n", fixed_pt);

        //printf("\n\n Iteration %d\n", i);
        //printParticles();
		i++;
    }while(i<NUM_ITERATIONS);

	//calcPrand();
	free(matrixN);
	free(matrixNBar);
	free(matrixNBarCopy);
}

/*
 **************************************
 * Reanimates a particle. 
 * Selects a random (weighted probability) node among the ones dominated by the particle, 
 * and moves the particle there. 
 * If there are no nodes dominated by this particle, one is selected at random. */ 
void particleRean(particle_type *p)
{
    int i; 
    int total_visits = 0; 
    int num_dominated_nodes = 0; 

    // count dominated nodes and total number of visits to them
    for (i = 0; i < graph->nvert; i++)
    {
        if(p->id == graph->nodes[i].idx_node.dominator)
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
            if(p->id == graph->nodes[i].idx_node.dominator)
            {
                step = step + matrixN[i][p->id]; 
                if(step >= weighted_prob)
                {
                    p->prev_node_id = p->curr_node_id; 
                    p->curr_node_id = i; 
                    p->energy = p->energy + DELTA; 
                    p->state = ACTIVE; 
                    matrixN[i][p->id]++;
					break;
                }
            }
        }    
    }
    else
    {
        int prob = rand()%(graph->nvert);
        p->prev_node_id = p->curr_node_id;
        p->curr_node_id = prob;
		p->energy = p->energy + DELTA; 
        p->state = ACTIVE;
        matrixN[prob][p->id]++;
    }
}

/*
 *******************************************
 * Computes the pref walk vector for a particle that currently
 * occupies node i and has to select one of its nodes to move to
 */
double *particlePrefWalk(particle_type *p)
{
	int i;
	double nbar_sum = 0.0;
	int num_neighbors = graph->nodes[p->curr_node_id].num_neighbors;
	adjlist_node_type *listPtr = graph->nodes[p->curr_node_id].head;
	double *pref_prob = (double*)malloc(num_neighbors*sizeof(double));

	CHECK((pref_prob == NULL), "Unable to allocate momory for pref_prob vector.");	

	for(i=0; i< num_neighbors; i++){
		nbar_sum = nbar_sum + matrixNBar[listPtr->id][p->id];
		listPtr = listPtr->next;
	}
	listPtr = graph->nodes[p->curr_node_id].head;
	for(i=0; i< num_neighbors; i++){
		pref_prob[i] = matrixNBar[listPtr->id][p->id]/nbar_sum;
		listPtr = listPtr->next;
    }

	return pref_prob;
}

/*
 ***********************************
 * Selects a new node for particle p to move to using weighted rand probaility
 * on the vector containing the transition probabilities for all neighbors of
 * the node p currently occupies.
 */
void selectNextNode(particle_type *p, double *tran_vector, int num_neighbors)
{
	double prob = (double)rand()/(double)RAND_MAX;
	int i;
    double step = 0.0;
	adjlist_node_type *listPtr = graph->nodes[p->curr_node_id].head;

//	printf("prob = %g\n", prob);
    for(i=0; i<num_neighbors; i++)
    {
		step = step + tran_vector[i];
//		printf("neighbor %d-> step = %g\n", i, step);
        if(step >= prob)
		{
		    p->prev_node_id = p->curr_node_id;
		    p->curr_node_id = listPtr->id;
            break;
        }
		listPtr = listPtr->next;
    }

}

/*
 *************************************
 * Picks a random number between 1 - num_neighbors, and places the particle at that node (=walks the list to that neighbor)
 * TODO: Make sure that you are not going back to the node that you just came from (unless you have no other choice)
 */ 
void particleRandomWalk(particle_type *p)
{
    int i; 
    adjlist_node_type *listPtr = graph->nodes[p->curr_node_id].head;

    assert(NULL != listPtr);
    
    int num_neighbors = graph->nodes[p->curr_node_id].num_neighbors;
    int neighbor_number = rand() % num_neighbors;

    for(i=0; i<neighbor_number; i++)
    {
        listPtr = listPtr->next;     
        assert(NULL != listPtr); 
    }
    
    p->prev_node_id = p->curr_node_id;
    p->curr_node_id = listPtr->id; 

}

/*
 **************************************
 * Calculates the energy level of a given node*
 */
void calcEnergy(particle_type *p)
{
	int dominator = graph->nodes[p->curr_node_id].idx_node.dominator;
    if(p->id == dominator)
        p->energy = p->energy + DELTA;
    else
        p->energy =  p->energy - DELTA;

    if(p->energy > OMEGA_MAX)
    {
        p->energy = OMEGA_MAX;
    }

    // Check if particle is exhausted
    if(p->energy <= OMEGA_MIN)
    {
        p->energy = OMEGA_MIN;
        p->state = EXHAUSTED;
    }
	else
		p->state = ACTIVE;

}

/*
 **************************************
 * Calculates the new dominator (particle) of the newly occupied, by particle j, node i
*/
void compNodeDominator(particle_type *p)
{
	// Recalculate the new dominator for that node
    int i;
	int max_visits = 1;
    int temp;
    int new_dominator = graph->nodes[p->curr_node_id].idx_node.dominator;;
    for(i=0; i<NUM_PARTICLES; i++)
    {
        temp = 0;
        temp = matrixN[p->curr_node_id][i];
        if(temp > max_visits)
        {
            max_visits = temp;
            new_dominator = i;
        }
    }
    graph->nodes[p->curr_node_id].idx_node.dominator = new_dominator;

}

/*
 **************************************
 * Computes N bar ( the relative frequency of visits of particle k
 * to node i) for particle k on node i
 */
double compNBar (int k, int i)
{
	int j;
	int total_visits = 0;
	//adjlist_node_type *listPtr = graph->nodes[i].head;
	
	for(j=0; j<NUM_PARTICLES; j++){
		//k-inclusive
		total_visits = total_visits + matrixN[i][j];
	}
	return (double)(matrixN[i][k]/total_visits);
}

/*
 **************************************
 *Computes matrixNBar containing the relative frequencies of \
 *visits of particle k to node i
 */
void compMatrixNBar()
{
	int i,j;
	int total_visits = 0;

	for(i=0; i < graph->nvert; i++){
		for(j=0; j<NUM_PARTICLES; j++){
		    //k-inclusive
			total_visits = total_visits + matrixN[i][j];
	    }
		for(j=0; j<NUM_PARTICLES; j++){
	        //k-inclusive
			matrixNBar[i][j] = (double)(matrixN[i][j]/total_visits);
	    }

	}
}

/*
 *************************************
 *Computes the infinity norm of ( NBar(t)-NBar(t-1)) in order to \
 *compare to EPSILON to approximate a fixed point
 */
double compFixPoint()
{
	int i,j, out;
	double max_diff = 0.0;
	double *diff_vect = NULL;
	double **diff_mat = NULL;

	diff_mat = create2DArray(graph->nvert, NUM_PARTICLES);
    out = (diff_mat == NULL);
    CHECK(out, "Unable to allocate memroy for diff_mat");
	
	diff_vect = (double*)malloc(graph->nvert*sizeof(double));
	out = (diff_vect == NULL);
    CHECK(out, "Unable to allocate memroy for diff_vect");	

	//Init the diff vector with all 0s
	bzero(diff_vect, (graph->nvert)*sizeof(double));

	//Compute the diff b/n matrixNBar(aka NBar(t)) and matrixNBarCopy (aka NBar(t-1)) 
	for(i=0; i < graph->nvert; i++)
		for(j=0; j < NUM_PARTICLES; j++)
			diff_mat[i][j] = matrixNBar[i][j] - matrixNBarCopy[i][j];
	printf("matrixNBarCopy\n");
	print2DArr(matrixNBarCopy, graph->nvert, NUM_PARTICLES);
	printf("matrixNBar\n");
	print2DArr(matrixNBar, graph->nvert, NUM_PARTICLES);
	//print2DArr(diff_mat, graph->nvert, NUM_PARTICLES);
	//Sum up the entries in each raw of the diff matrix
    for(i=0; i < graph->nvert; i++)
        for(j=0; j < NUM_PARTICLES; j++)
            diff_vect[i] = diff_vect[i] + diff_mat[i][j];

	//find the max value in diff_vect
//	max_diff = diff_vect[max_array(diff_vect, graph->nvert)]; 	

	free(diff_vect);
	free(diff_mat);
	return max_diff;
}	

/*
 *************************************
 * Initializes matrix N with 1 if the particle k did not 
 * originate (during initializatin) at node i, and with 2, otherwise 
 */
void initMatrixN(int rows, int cols)
{
	int i, k;
	
	printf("\nInitialized matrixN:\n");
	printf("--------------------\n");
	for(i=0; i<rows; i++){
		printf("%d->\t", i);
		for(k=0; k<cols; k++){
			if(particles[k].curr_node_id == i)
				matrixN[i][k] = 2.0;
			else
				matrixN[i][k] = 1.0;
			printf("%g\t", matrixN[i][k]);
		}
		printf("\n");
	}
	printf("*************************\n");
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

/*
 *************************************
 *Prints out an array*/
void print2DArr(double **arr, int rows, int cols)
{
	int k,i;

	//Print to file
	for(k=0; k<rows; k++){
		printf("%d->\t", k);
		for(i=0; i<cols; i++){
			printf("%g\t", arr[k][i]);
		}
		printf("\n*************\n");
	}

}

/*
 *************************************
 *Finds max value in an array of floats and returns its INDEX. It assumes that there is at least 1
 *non-negative element in the array.
*/
int max_array(double *arr, int arrLen)
{
   int i, idx;
   double max=0.0;
   for (i=0; i<arrLen; i++){
	 if (arr[i]>max){
	    max=arr[i];
	    idx = i;
	 }
   }
   return(idx);
}
