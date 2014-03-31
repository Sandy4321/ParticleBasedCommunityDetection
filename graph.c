/*
 * graph.c
 * Author: apetkova
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "main_driver.h"

#define NEWLINE 10


/*
 *Reads data from input file and builds a graph
 *based on it
 */
int readGraph(char *filename)
{
	int i,j,c;
	int vertex, neighbor, num_nodes;
	double weight, rel_weight, total_weight;
	char buffer[10];
	char *end;
	FILE *fin;

	//open data file for reading
	fin = fopen(filename, "r");
		if( fin == NULL ) {
			printf(" Error, could not open %s, abort\n", filename);
        	return 0;
    	}

	//read in the number of nodes	
	i=0;
	while((c = fgetc(fin))!= EOF) {
		if(c == NEWLINE)
			break;
		if(c!='%'){
			buffer[i] = c;
			i++;
		}
	}
	//convert the number of nodes string to int
	num_nodes = strtod(buffer, &end);
	printf("num_nodes = %d\n", num_nodes);
	createGraph(num_nodes);

	while (!feof(fin)) {
		if (fscanf(fin, "%d %d", &vertex, &neighbor) != 2)
			break;
		printf("%d %d\n", vertex, neighbor);
		addEdge(vertex-1, neighbor-1);
	}
	
	//printGraph(graph);			
}	

/* 
 *Creates an edge list node
 */
adjlist_node_type* createNode(int v)
{
	int i;
	
    adjlist_node_type* newNode = (adjlist_node_type*)malloc(sizeof(adjlist_node_type));
	//Check if memeory allocation was successful
    i = (newNode == NULL);
	CHECK(i, "Unable to allocate memory for new node");
 
    newNode->id = v;
    newNode->weight = 1;
    newNode->next = NULL;
 
    return newNode;
}
 
/* 
 *Creates an undirected graph with n vertices
 */
int createGraph(int n)
{
    int i, j=0;
	
    graph = (graph_type*)malloc(sizeof(graph_type));
    //check if memory allocation was successful
    j = (graph == NULL);
    CHECK(j, "Unable to allocate memory for graph");
    graph->nvert = n;
 
    // Create an array of adjacency lists
    graph->nodes = (adjlist_type*)malloc(n * sizeof(adjlist_type));
    j = ((graph->nodes) == NULL);
	CHECK(j, "Unable to allocate memory for adjacency list array");
 
	//Init each nodes list (array)
    for(i = 0; i < n; i++)
    {
	  	graph->nodes[i].idx_node.id = i;
	  	graph->nodes[i].idx_node.dominator = -1;
        graph->nodes[i].head = NULL;
        graph->nodes[i].num_neighbors = 0;
    }

    return 0; 
}
 
/*
 *Destroys the graph
 */
void destroyGraph()
{
	int i;

    if(graph)
    {
        if(graph->nodes)
        {
            //Free up the nodes
            for (i = 0; i < graph->nvert; i++)
            {
                adjlist_node_type* listPtr = graph->nodes[i].head;
                while (listPtr)
                {
                    adjlist_node_type* tmp = listPtr;
                    listPtr = listPtr->next;
                    free(tmp);
                }
            }
            //Free the adjacency list array
            free(graph->nodes);
        }
        //Free graph
        free(graph);
    }
}
 
/* 
 *Inserts an edge in the graph
 *	
*/
void addEdge(int src, int dest)
{
    // Add an edge from src to dst in the adjacency list
    adjlist_node_type* newNode = createNode(dest);
    newNode->next = graph->nodes[src].head;
    graph->nodes[src].head = newNode;
    graph->nodes[src].num_neighbors++;
 
    // Since graph is undirected, add an edge from dest to src as well
    newNode = createNode(src);
    newNode->next = graph->nodes[dest].head;
    graph->nodes[dest].head = newNode;
    graph->nodes[dest].num_neighbors++; 
}

/*
 *****************************************
 * Checks if node j is a neighbor of node i
 */
int isNeighbor(int i, int j)
{
	int k;
	adjlist_node_type *listPtr = graph->nodes[i].head;

	for(k=0; k< graph->nodes[i].num_neighbors; k++){
		if(listPtr->id == graph->nodes[i].idx_node.id)
			return 1;
		listPtr = listPtr->next;
	}
	return 0;
}

 
/*
 *****************************************
 *Prints out the graph as an adjacency list
 */
void printGraph()
{
    int i;
	
	printf("\nGraph's Adjacency List:\n");
	printf("-----------------------\n");
    for (i = 0; i < graph->nvert; i++)
    {
        adjlist_node_type* listPtr = graph->nodes[i].head;
        printf("%d: ", graph->nodes[i].idx_node.id);
        while (listPtr)
        {
            printf("->%d", listPtr->id);
            listPtr = listPtr->next;
        }
        printf("\tdominator: %d \n", graph->nodes[i].idx_node.dominator);
    }
}

