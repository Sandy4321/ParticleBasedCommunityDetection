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

//adjlist_node_type *adjlist_node_ptr; 
//adjlist_type *adjlist_ptr; 

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
		printf("c = %s", buffer);
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
    graph->neighbors = (adjlist_type*)malloc(n * sizeof(adjlist_type));
    j = ((graph->neighbors) == NULL);
	CHECK(j, "Unable to allocate memory for adjacency list array");
 
	//Init each neighbors list (array)
    for(i = 0; i < n; i++)
    {
	  	graph->neighbors[i].idx_node.id = i;
	  	graph->neighbors[i].idx_node.dominator = -1;
        graph->neighbors[i].head = NULL;
        graph->neighbors[i].num_neighbors = 0;
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
        if(graph->neighbors)
        {
            //Free up the nodes
            for (i = 0; i < graph->nvert; i++)
            {
                adjlist_node_type* listPtr = graph->neighbors[i].head;
                while (listPtr)
                {
                    adjlist_node_type* tmp = listPtr;
                    listPtr = listPtr->next;
                    free(tmp);
                }
            }
            //Free the adjacency list array
            free(graph->neighbors);
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
    newNode->next = graph->neighbors[src].head;
    graph->neighbors[src].head = newNode;
    graph->neighbors[src].num_neighbors++;
 
    // Since graph is undirected, add an edge from dest to src as well
    newNode = createNode(src);
    newNode->next = graph->neighbors[dest].head;
    graph->neighbors[dest].head = newNode;
    graph->neighbors[dest].num_neighbors++; 
}
 
/*
 *Prints out the graph as an adjacency list
 */
void printGraph()
{
    int i;
    for (i = 0; i < graph->nvert; i++)
    {
        adjlist_node_type* listPtr = graph->neighbors[i].head;
        printf("\n%d: ", graph->neighbors[i].idx_node.id);
        while (listPtr)
        {
            printf("->%d", listPtr->id);
            listPtr = listPtr->next;
        }
        printf("\tdominator: %d ", graph->neighbors[i].idx_node.dominator);
    }
}

