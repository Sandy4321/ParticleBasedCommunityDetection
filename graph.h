/*
 * graph.h
 * Author: apetkova
 *
 */

#ifndef GRAPH_H_
#define GRAPH_H_


 // edgelist struct
typedef struct neighbor
{
    int id;                 // node id of the neighbor
  //  double weight;         // weight of edge b/n node & neighbor
    struct neighbor *next;  //pointer to another neighbor
} neighbor_type;

// edgelist node struct
typedef struct adjlist_node
{
    int id;
    int dominator;     // which particle dominates this node
    int num_neighbors;
	int label;
    neighbor_type *head;
} adjlist_node_type;

// describes a stripped particle profile \
   used in matrix N
typedef struct mini_particle
{
    int id;
	int nvisits;
    struct mini_particle *next;
} mini_particle_type;

// stripped down version of the node \
   profile to be used in the matrix N \
   array of lists
typedef struct mini_node
{
	int id;
	mini_particle_type *head; 
} mini_node_type;

// graph struct
typedef struct graph
{
    int nvert;      // |V|
    int nedges;          // |E|
    adjlist_node_type *nodes;
} graph_type;

graph_type *graph;

// matrix N struct
typedef struct matrixN
{
	mini_node_type *nodes;
} matrixN_type;

matrixN_type *matrixN;

//Reads data from input file and builds a graph based on it
int readGraph(char *filename);

int readLabels(char *filename);

//Creates an undirected graph with n vertices
int createGraph(int n);

//Destroys the graph
void destroyGraph();
 
//Inserts an edge in the graph
void addEdge(int src, int dest);

//Checks if node j is a neighbor of node i
int isNeighbor(int i, int j);
 
//Prints the graph as an adjacency list
void printGraph();



#endif
