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

double delta;
double epsilon;

int *countN;
int *countNOld;


/*
 **************************************
 * Drives the community detection algorithm
 */
void runDetection(int num_particles, double lambda, double d, double e, int weighted_rean)
{

	int i=0, j, k=0, out=0;
	int success = 0;
	int num_neighbors = 0;
	double rand_prob = 0.0;
	double fixed_pt = 0.0;
	double *pref_prob = NULL;
	double *tran_vector = NULL;
	neighbor_type *listPtr = NULL;
	delta = d;
	epsilon = e;
	int rean_ctr = 0;

    assert(graph != NULL);
    srand(time(NULL));
//    srand(0);
    countN = (unsigned int*)malloc(graph->nvert*sizeof(unsigned int));
    out = (countN == NULL);
    CHECK(out, "Unable to allocate memroy for countN");

    countNOld = (unsigned int*)malloc(graph->nvert*sizeof(unsigned int));
    out = (countNOld == NULL);
    CHECK(out, "Unable to allocate memroy for countNOld");

    createMatrixN(graph->nvert, num_particles);
    printMatrixN(graph->nvert);

    initCountN(graph->nvert, num_particles);
    printf("\nInitialized countN\n-----------------\n");

    initUpdateParticles(num_particles);
    printParticles(num_particles);

	printf("\nRUNNING DETECTION ALG.....\n");
	// Main descrete-time driving loop. 
	// In each iteration, each particle performs a walk
    do
    {
        for(j=0; j < num_particles; j++)		// for EACH PARTICLE 
        {
			if(particles[j].energy > OMEGA_MIN){
				//find the num of neighbors of the node particle j \
				currently occupies
	//			printf("\n\n Iteration %d\tparticle %d(%d)\n", i, j, particles[j].id);
				num_neighbors = graph->nodes[particles[j].curr_node_id].num_neighbors;
	//			printf("num_neighbors = %d\n", num_neighbors);
				listPtr = graph->nodes[particles[j].curr_node_id].head;
				assert(NULL != listPtr);
				
				//compute the rand probability to visit a \
				neighbor of nodes[particles[j].curr_node_id]
                rand_prob = 1/(double)num_neighbors;
				
				//compute the pref walk vector
				pref_prob = particlePrefWalk(&particles[j]);
				
				//printf("pref_prob", pref_prob);
				//compute transition vector
				tran_vector = (double*)malloc(num_neighbors*sizeof(double));
				out = (tran_vector == NULL);
			    CHECK(out, "Unable to allocate memroy for tran_vector");
				for(k=0; k < num_neighbors; k++){
					tran_vector[k] = (1-lambda)*rand_prob + lambda*pref_prob[k];
				//	printf("%g | ", tran_vector[k]);
				}
			//	printf("\n");
				
				//make a final selection of the node to transition to
				success = 0;
				while(!success)
					success = selectNextNode(&particles[j], tran_vector, num_neighbors);

				//update particle energy level after move
				calcEnergy(&particles[j]);

				//calculate the new dominator of the node particle j just moved to
				compNodeDominator(&particles[j], num_particles);
				
				free(tran_vector);
				free(pref_prob);
			}
            else{
				if(particles[j].energy < OMEGA_MIN)
					particles[j].energy = OMEGA_MIN;
				// if particle's energy is drained - reanimate
                particleRean(&particles[j], weighted_rean);
				rean_ctr++;
			}
        }
        fixed_pt = compFixPoint(num_particles);

        syncCountN(graph->nvert);
	    //printf("FIXED_PT = %g\n", fixed_pt);
        //printf("\n\n Iteration %d\n", i);

		i++;
        if((i%1000000)==0){
            printf("\n\n Iteration %d\n", i);
            printf("fixed_pt = %8.8g\n", fixed_pt);

        }

    // (0.0 == fixed_pt) whenever multiple particles visit the same node. 
    // We allow this to avoid situations in which particles are stuck on a node
    // and also to avoid fixed_pt to be reached prematurely whenever Nbar - NbarCopy = 0
    // due to multiple particles visiting the same node. 
    }while((fixed_pt > epsilon)||(0.0 == fixed_pt));
    //}while(i < num_iterations);
	printf("WEIGHTED_REAN_POLICY = %d\n", weighted_rean);	
    printf("\n\nfinal_no_iterations: %d\n", i);
    printf("fixed_pt = %g\n", fixed_pt);
	printf("no_reanimated_particles: %d\n", rean_ctr);
	
	printf("avg_no_particles_per_node: %d\n", avgNumParticlesPerNode(num_particles));
	printf("lambda = %g\t delta = %g\t epsilon = %g\n", lambda, delta, epsilon);
	printf("avg_max_frequncy = %g\n", compAvgMaxFrequency(num_particles));

    free(countN);
    free(countNOld);
    deleteMatrixN();
}

/*
 **************************************
 * Reanimates a particle. 
 * Selects a random (weighted probability) node among the ones dominated by the particle, 
 * and moves the particle there. 
 * If there are no nodes dominated by this particle, one is selected at random. */ 
void particleRean(particle_type *p, int weighted_rean)
{
    int i; 
    int total_visits = 0; 
    int num_dominated_nodes = 0;
	//printf("I am in particleRean\n");
    // count dominated nodes and total number of visits to them
    for (i = 0; i < graph->nvert; i++)
    {
        if(p->id == graph->nodes[i].dominator)
        {
            total_visits = total_visits + getMatrixNCount(i,p->id); 
            num_dominated_nodes++; 
        }
    }

    // perform weighted random selection of dominated node
    if(num_dominated_nodes > 0)
    {
        int weighted_prob = rand()%total_visits; 
        int step = 0; 
		int ctr = 0;
	
	    for(i=0; i<graph->nvert; i++)
        {
			if((weighted_rean == 0) && (p->id == graph->nodes[i].dominator)){
				if(ctr == weighted_prob){
					p->prev_node_id = p->curr_node_id;
					p->curr_node_id = i;
					p->energy = p->energy + delta;
                    p->prev_count = getMatrixNCount(i,p->id);
                    incrementMatrixN(i, p->id);
					break;
				}
				ctr++;

			}	
           if((weighted_rean == 1) && (p->id == graph->nodes[i].dominator))
            {
                step = step + getMatrixNCount(i,p->id); 
                if(step >= weighted_prob)
                {
                    p->prev_node_id = p->curr_node_id; 
                    p->curr_node_id = i; 
                    p->energy = p->energy + delta; 
                    p->prev_count = getMatrixNCount(i,p->id);
                    incrementMatrixN(i, p->id);
					break;
                }
            }
        }    
    }
    else
    {
		printf("RAND REAN\n");
        int prob = rand()%(graph->nvert);
        p->prev_node_id = p->curr_node_id;
        p->curr_node_id = prob;
		p->energy = p->energy + delta; 
        p->prev_count = getMatrixNCount(prob,p->id);
        incrementMatrixN(prob,p->id);
    }
}

/*
 *******************************************
 * Computes the pref walk vector for a particle that currently
 * occupies node i and has to select one of its nodes to move to
 */
double *particlePrefWalk(particle_type *p)
{
	int i=0, j=0;
	double nbar_sum = 0.0;
	neighbor_type *listPtr = NULL;
	int num_neighbors = graph->nodes[p->curr_node_id].num_neighbors;
	listPtr = graph->nodes[p->curr_node_id].head;
	j = (listPtr == NULL);
    CHECK(j, "listPtr is NULL");
	//printf("node_id =%d\n", graph->nodes[p->curr_node_id].id);
    //printf("head_node_id =%d\n", graph->nodes[p->curr_node_id].head->id);
	double *pref_prob = (double*)malloc(num_neighbors*sizeof(double));
	CHECK((pref_prob == NULL), "Unable to allocate momory for pref_prob vector.");	

	for(i=0; i< num_neighbors; i++){
		nbar_sum = nbar_sum + getNBar(listPtr->id,p->id);
		listPtr = listPtr->next;
	}
	listPtr = graph->nodes[p->curr_node_id].head;
	for(i=0; i< num_neighbors; i++){
        pref_prob[i] = getNBar(listPtr->id,p->id)/nbar_sum;
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
int selectNextNode(particle_type *p, double *tran_vector, int num_neighbors)
{
	int success = 0;
	double prob = (double)rand()/(double)RAND_MAX;
	int i, j, k;
    double step = 0.0;
	neighbor_type *listPtr = graph->nodes[p->curr_node_id].head;

//	printf("prob = %g\n", prob);
    for(i=0; i<num_neighbors; i++)
    {
		step = step + tran_vector[i];
//		printf("neighbor %d-> step = %g\n", i, step);
        if((step >= prob))
		{	

			if((p->prev_node_id == listPtr->id)&&(num_neighbors > 1))
			{
				break;
			}
            
			else{

                /*
                // Issue if particle is occupying the only available neighbor. 
                for(j=0; j< p->id; j++)
                {
                    if(particles[j].curr_node_id == listPtr->id)
                    {
                        return 0; 
                    }

                }
                */
				p->prev_node_id = p->curr_node_id;
				p->curr_node_id = listPtr->id;
                p->prev_count = getMatrixNCount(p->curr_node_id,p->id);
                incrementMatrixN(p->curr_node_id, p->id); 
				success = 1;
				break;
			}
        }
		listPtr = listPtr->next;
    }
	return success;
}

/*
 *************************************
 * Picks a random number between 1 - num_neighbors, and places the particle at that node (=walks the list to that neighbor)
 * TODO: Make sure that you are not going back to the node that you just came from (unless you have no other choice)
 */ 
void particleRandomWalk(particle_type *p)
{
    int i; 
    neighbor_type *listPtr = graph->nodes[p->curr_node_id].head;

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
	int dominator = graph->nodes[p->curr_node_id].dominator;
    if(p->id == dominator)
        p->energy = p->energy + delta;
    else
        p->energy =  p->energy - delta;

    if(p->energy > OMEGA_MAX)
    {
        p->energy = OMEGA_MAX;
    }

    // Check if particle is exhausted
    if(p->energy <= OMEGA_MIN)
    {
        p->energy = OMEGA_MIN;
       // p->state = EXHAUSTED;
    }
//	else
//		p->state = ACTIVE;

}

/*
 **************************************
 * Calculates the new dominator (particle) of the newly occupied, by particle j, node i
*/
void compNodeDominator(particle_type *p, int num_particles)
{
	// Recalculate the new dominator for that node
    int i;
	int max_visits = 1;
    int temp = 0;
    int new_dominator = graph->nodes[p->curr_node_id].dominator;
    for(i=0; i<num_particles; i++)
    {
        temp = 0;
        temp = (int)getMatrixNCount(p->curr_node_id,i);
        if(temp > max_visits)
        {
            max_visits = temp;
            new_dominator = i;
        }
    }
    graph->nodes[p->curr_node_id].dominator = new_dominator;

}

double compAvgMaxFrequency(int num_particles)
{
	int i,j;
	double max_freq = 0.0;
	double temp = 0.0;
	double sum = 0.0;

	for(j=0; j< graph->nvert; j++){
		for(i=0; i<num_particles; i++)
		{
			temp = 0;
			temp = getNBar(j,i);
			if(temp > max_freq)
			{
				max_freq = temp;
			}
		}
		sum += max_freq;
	}
	
	return (sum/(double)graph->nvert);
}

/*
 *************************************
 *Computes the infinity norm of ( NBar(t)-NBar(t-1)) in order to \
 *compare to epsilon to approximate a fixed point
 */
double compFixPoint(int  num_particles)
{
	int i,j, out=0;
	double max_diff = 0.0;
	double *diff_vect = NULL;

	diff_vect = (double*)malloc(graph->nvert*sizeof(double));
	out = (diff_vect == NULL);
    CHECK(out, "Unable to allocate memroy for diff_vect");	
	//Init the diff vector with all 0s
	bzero(diff_vect, (graph->nvert)*sizeof(double));

    for(i=0; i < graph->nvert; i++)
    {
        diff_vect[i] = 0.0; 
        for(j=0; j < num_particles; j++)
        {
            if(i == particles[j].curr_node_id)
                diff_vect[i] = diff_vect[i] + ABS(getNBar(i,j) - getNBarPrevParticle(i,j));
            else
                diff_vect[i] = diff_vect[i] + ABS(getNBar(i,j) - getNBarPrev(i,j));
        }
    }

	//find the max value in diff_vect
	max_diff = diff_vect[max_array(diff_vect, graph->nvert)]; 	

	free(diff_vect);
	return max_diff;
}	
/*
 *************************************
 * Initializes matrix N with 1 if the particle k did not
 * originate (during initializatin) at node i, and with 2, otherwise
 */
void createMatrixN(int rows, int cols)
{
    int i, j, out;

    // Allocate memory for the graph pointer
    matrixN = (matrixN_type*)malloc(sizeof(matrixN_type));
    //check if memory allocation was successful
    out = (matrixN == NULL);
    CHECK(out, "Unable to allocate memory for matrix N.");

    matrixN->nodes = (mini_node_type*)malloc( sizeof(mini_node_type) * rows );
    //check if memory allocation was successful
    out = ((matrixN->nodes) == NULL);
    CHECK(out, "Unable to allocate memory for the nodes of matrix N.");

    printf("\nCreating matrixN:\n");
    printf("--------------------\n");

    //initialize all pointers to particle lists to NULL
    for(i = 0; i < rows; i++)
        matrixN->nodes[i].head = NULL;

    //add particles to the particle lists of their \
      initial node hosts
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            if(particles[j].curr_node_id == i){
                addMiniParticle(i, j); 
            }
        }
    }
}

void deleteMatrixN()
{
    int i;

    if(matrixN)
    {
        if(matrixN->nodes)
        {
            //Free up the nodes
            for (i = 0; i < graph->nvert; i++)
            {
                mini_particle_type* listPtr = matrixN->nodes[i].head;
                while (listPtr)
                {
                    mini_particle_type* tmp = listPtr;
                    listPtr = listPtr->next;
                    free(tmp);
                }
                free(listPtr);
            }
            //Free the adjacency list array
            free(matrixN->nodes);
        }

        free(matrixN);
    }
}

void addMiniParticle(int i, int j)
{
    mini_particle_type* newPart = (mini_particle_type*)malloc(sizeof(mini_particle_type));
    assert(newPart != NULL);
    newPart->id = j;
    newPart->nvisits = 2;
    newPart->next = matrixN->nodes[i].head;
    //attach mini particle to the head of the particle list of node i
    matrixN->nodes[i].head = newPart;
}

void printMatrixN(int rows)
{
    int i; 

    mini_particle_type* newPart;
    for(i=0; i<rows; i++){
        printf("[%d]", i);
        newPart = matrixN->nodes[i].head;
        while(newPart!=NULL){
            printf("-->\n");
            printf("%d: %d", newPart->id, newPart->nvisits);
            newPart = newPart->next;
        }
        printf("-->NULL\n");
    }
    printf("*************************\n");

}

mini_particle_type* getMatrixN(int i, int j)
{
    mini_particle_type* head = matrixN->nodes[i].head;

    while(head != NULL)
    {
        if(head->id == j)
            return head;
        head = head->next; 
    }
    
    // Particle has not visited this node
    return NULL;
}

double getMatrixNCount(int i, int j)
{
    mini_particle_type* p = getMatrixN(i,j); 
    if(NULL != p)
        return (double)(p->nvisits);
    else
        return 1.0; 
}

void initCountN(int rows, int cols)
{
    int i, k;
    for(i=0; i<rows; i++){
        countN[i] = 0;
        for(k=0; k<cols; k++){
            countN[i] = countN[i] + (int)getMatrixNCount(i,k);
        }
        countNOld[i] = countN[i];
        printf("countN[%d]: %d\n", i, countN[i]);
    }
}



/*
 *************************************
Creates a multidimensional data structure according to passed size parameters
*/
double **create2DArray( int row, int col ) {
    double ** array_ptr = NULL;
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
   int i;
   int idx = 0;
   double max=0.0;
   for (i=0; i<arrLen; i++){
	 if (arr[i]>max){
	    max=arr[i];
	    idx = i;
	 }
   }
   return(idx);
}

/*
 **************************************
 * Computes the avg number of particle visitors per node
 *
 */
int avgNumParticlesPerNode(int num_particles)
{
	int i, j;
	int avg_no = 0;

	for(i=0; i < graph->nvert; i++)
		for(j=0; j < num_particles; j++)
			if(getMatrixNCount(i,j) > 1.0)	//matrix N was initialized with 1s and 2s not 0s
				avg_no++;	


	return (avg_no/graph->nvert);
}



/*
 ***************************************
 * Print each cluster. We have as many clusters as we have particles in this algorithm
 */
void printClusters(int num_particles, int label_flag)
{

    int i, j, no_correct = 0;
	int out = 0, max = 0;
	double accuracy = 0.0;
	int *labels = NULL;

	//Create an array of labels the size of the number of particles. \
	It is an array of counters that keep track of how many nodes in \
	a cluster are of a certain label after clustering is done. \
	If clustering was successful all nodes in a cluster should be labeled \
	the same and thus only one of the counters in the array should be > 0.
	labels = (int*)malloc(num_particles*sizeof(int));
	out = (labels == NULL);
	CHECK(out, "Unable to allocate memroy for labels.");	
	bzero(labels, num_particles*sizeof(int));
 
	printf("\n");
    for(i=0; i< num_particles; i++)
   //int i;
    {
        printf("Cluster %d: ", i); 
        for(j=0; j< graph->nvert; j++)
        {
		    if(graph->nodes[j].dominator == i){
			    printf("%d, ", j+1); 
				labels[graph->nodes[j].label]++;
			}								
        }
        printf("\n");

		for(j=0; j < num_particles; j++){
			if(labels[j]>max)
				max = labels[j]; 
		}
		no_correct += max;
		bzero(labels, num_particles*sizeof(int));
		max = 0;
    }

	printf("\n");

	if(label_flag == 1) {
		for(i=0; i< num_particles; i++)
	//int i;
		{
			printf("Cluster %d: ", i); 
			for(j=0; j< graph->nvert; j++)
			{
                if(graph->nodes[j].dominator == i)
                    printf("%d,", graph->nodes[j].label); 
			}
			printf("\n"); 
		}
	}
	
	printf("accuracy: %.2f\n", ((double)no_correct/graph->nvert));
	free(labels);
}

void incrementMatrixN(int i, int j)
{
    double count;    
    mini_particle_type* p; 
    
    countN[i]++;

    p = getMatrixN(i,j); 
    if(NULL == p)
        addMiniParticle(i,j); 
    else
        p->nvisits++; 
}

void syncCountN(int rows)
{
    int i ;
    for(i=0; i<rows; i++)
        countNOld[i] = countN[i];
}

double getNBar(int i, int j)
{
    double count = (double)countN[i];
    double nbar = getMatrixNCount(i,j)/count;
    return nbar;
}

double getNBarPrev(int i, int j)
{
    double count = (double)countNOld[i];
    double nbar = getMatrixNCount(i,j)/count;
    return nbar;
}

double getNBarPrevParticle(int i, int j)
{
    double count = (double)countNOld[i];
    double prev = (double)particles[j].prev_count;
    double nbar = prev/count;
    return nbar;
}

                                            
