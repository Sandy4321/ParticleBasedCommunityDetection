/*
 * graph.h
 * Author: apetkova
 *
 */

#ifndef GRAPH_H_
#define GRAPH_H_

// edgelist node struct
typedef struct adjlist_node
{
    int id;
    double weight;
    double rel_weight;

    int dominator;     // which particle dominates this node

    struct adjlist_node *next;
} adjlist_node_type;

 // edgelist struct
typedef struct adjlist
{
    int num_neighbors;
    adjlist_node_type idx_node;                 //the actual vertex
    adjlist_node_type *head;                    //pointer to the beginning of \
                                                  the neighbor list of the vertex
} adjlist_type;

// graph struct
typedef struct graph
{
    int nvert;      // |V|
    int nedges;          // |E|
    adjlist_type *nodes;
} graph_type;

graph_type *graph; 

//Reads data from input file and builds a graph based on it
int readGraph(char *filename);

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
