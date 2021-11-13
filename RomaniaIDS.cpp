#include <stdio.h>
#include<iostream> 
#include <stdlib.h> 
#include <list>
#include <vector>

using namespace std;

//The number of vertices stored in our graph
int vertexCount = 0;
//A 2D array of edges in the format graph[vertexA][vertexB] = distance between vertexA and vertexB
int** graph;
//A list containing all the adjacent nodes for each node
list<int> *adjacentNodes;
//The output logging file. Don't worry about this one
FILE* output;

/*
AddEdge: Add an edge to the graph from vertex u to vertex v.
Arugments:
	int u: One of the vertices that form the edge
	int v: The other vertice that forms the edge
	int dist: The distance between u and v
Adds an edge to the graph from vertex u to vertex v with the given distance dist.
The graph is represented a a 2D array of integers, where each integer is a distance.
When an edge is added from u to v an edge from v to u is also saved in memory.

NOTE: modified to keep track of adjacent nodes.

Returns void: No value returned
*/
void AddEdge(int u, int v, int dist)
{
	graph[u][v] = dist;
	graph[v][u] = dist;
	adjacentNodes[u].push_back(v);
	adjacentNodes[v].push_back(u);
}

/*
ReadGraphFile: Read graph data in from a file to populate our graph structure.
Arguments:
	char* fname: The name of the file to read
Reads graph data in from a file to populate our graph structure. The format of the file
defines a number of vertices at the top of the file, and edges for the graph for each
line afterwards in the format (U, V, DIST).
Returns bool: Returns true if the file was opened and read successfuly, and false if the file could not be read
*/
bool ReadGraphFile(char* fname)
{
	FILE* file = fopen(fname, "r");
	if (file)
	{
		rewind(file);
		int a, b, d;
		
		fscanf(file, "%d", &vertexCount);
		//fscanf_s(file, "%d", &vertexCount);

		graph = new int*[vertexCount];
		adjacentNodes = new list<int>[vertexCount];
		for (int u = 0; u < vertexCount; u++)
		{
			graph[u] = new int[vertexCount];

			for (int v = 0; v < vertexCount; v++)
			{
				graph[u][v] = 0;
			}
		}

		while (fscanf(file, "%d %d %d", &a, &b, &d) != EOF)
//		while (fscanf_s(file, "%d %d %d", &a, &b, &d) != EOF)
		{
			AddEdge(a, b, d);
		}

		fclose(file);

		return true;
	}
	else
	{
		printf("Failed to read file...\n");
		return false;
	}
}

/*
LogVertex: Prints the current vertex traveled to and saves the output to a file
Arguments:
	int vert: The vertex that you are currently visiting
Returns void: No values returned
*/
void LogVertex(int vert)
{
	printf("%d\n", vert);
	fprintf(output, "%d\n", vert);
}

/*
SetartLogging: Opens the logging file for later writing
Arguments:
	None
Pretty much just a wrapper for fopen
Returns void: No values returned
*/
void StartLogging()
{
	output = fopen("log.txt", "w");
}

/*
EndLogging: Closes the logging file properly
Arguments:
	None
Pretty much just a wrapper for fclose so that we make sure you close the right file.
Returns void: No values returned
*/
void EndLogging()
{
	fclose(output);
}
/*
IDS_helper: A helper function for IDS.
Arguments:
	int start: The starting city node 
	int end: The goal city node
	int depth_limit: the depth the current search will stop at
	bool *&visitedNodes: an array that stores whether a node has already been visited
	int*& predecessor: an array that stores the predecessor node of each node in the graph
Helps IDS by running a recursive depth first search for a specified depth limit.
Returns bool: true if solution is found, false otherwise
*/
bool IDS_helper(int start, int end, int depth_limit, bool *&visitedNodes/*, int *&predecessor*/)
{
	// only output newly traversed nodes
	if(visitedNodes[start] != true)
	{
		LogVertex(start);
		visitedNodes[start] = true;
	}

	// if the goal node is reached, we've found the solution
	if(start==end)
	{
		return true;
	}

	// if the depth limit is reached, there is no solution at current depth
	if(depth_limit <= 0)
	{
		return false;
	}

	// for all nodes adjacent to current node, appropriately update predecessor array
	// and perform depth first seach using IDS_helper decrementing the depth limit
	for(list<int>::iterator it = adjacentNodes[start].begin(); it != adjacentNodes[start].end(); ++it)
	{
			// need to ensure nodes dont become predecessors of each other
			// will cause infinite loop when solving for optimal path
			/*bool not_pred = true;
			int crawl = start;
			while(predecessor[crawl] != -1)
			{	if(*it == crawl)
				{
					not_pred = false;
				}
				crawl = predecessor[crawl]; 
				cout << predecessor[crawl] << endl;
			}

			if(*it != predecessor[start])
			//if(not_pred == true && start != )
			{
				predecessor[*it] = start;
				//cout << *it << " " << predecessor[*it] << endl;
			}*/

			if(IDS_helper(*it, end, depth_limit-1, visitedNodes/*, predecessor*/) ==  true)
			{
				return true;
			}
	}
	
	return false;
}
/*
IDS: Performs an iterative depth first search
Arguments:
	int start: The starting city node 
	int end: The goal city node
	int max_depth_limit: the maximum depth the search will stop at
	bool *&visitedNodes: an array that stores whether a node has already been visited
	int*& predecessor: an array that stores the predecessor node of each node in the graph
This function performs an iterative depth first search. For each iteration it calls IDS_helper and increments the depth limit each time.
Returns bool: true if solution is found, false otherwise
*/
bool IDS(int start, int end, int max_depth_limit, bool *&visitedNodes/*, int *&predecessor*/)
{
	// from 0 to maximum depth limit perform a depth first search at each depth limit
	for(int i=0; i<max_depth_limit; i++)
	{
		cout << "Limit = " << i << ":" << endl;
		if(IDS_helper(start, end, i, visitedNodes/*, predecessor*/) == true)
		{
			return true;
		}
		// clear predecessor array every iteration
		for(int i=0;i<vertexCount;i++)
		{
			visitedNodes[i] = false;
			//predecessor[i] = -1;
		}
	}
	return false;
}

int main(int argc, char** argv)
{
	//Setup
	//-----------------------------------
	if (argc == 4)
	{
		if (!ReadGraphFile(argv[1]))
			return 2;
	}
	else
	{
		printf("No file argument given...\n");
		return 1;
	}
	StartLogging();
	//-------------------------------------

	int startCity = atoi(argv[2]);
	int endCity = atoi(argv[3]);

	bool *visitedNodes = new bool[vertexCount]; // create a boolean array to keep track of visited nodes
	int* predecessor = new int[vertexCount];  // keeps track of parent nodes
	for(int i=0;i<vertexCount;i++)
	{
		visitedNodes[i] = false;
		predecessor[i] = -1;
	}

	// check if iterative deepening search finds a solution,
	// if it does we can use the predecessor array to find 
	// the path it found
	if(IDS(startCity, endCity, 100, visitedNodes/*, predecessor*/))
	{
		// was not able to output path for iterative deepening search
		/*vector<int> path; // stores shortest path
		int crawl = endCity; // start at end city and work backwards through pred array
		path.push_back(crawl); 
		while (predecessor[crawl] != -1) 
		{
			cout << predecessor[crawl] << endl;
			path.push_back(predecessor[crawl]); 
			crawl = predecessor[crawl]; 
		} 
		// printing path from source to destination 
		cout << "Potential path is: "; 
		for (int i = path.size() - 1; i > 0; i--) 
		{
			cout << path[i] << "->"; 
		}
		cout << path[0] << endl;*/
		cout << "There exists a path from start to end city." << endl; 
	}
	else
	{
		cout << "No paths from start to end city." << endl; 
	}

	//Cleanup
	//-------------------------------------
	EndLogging();
	getchar();
	//-------------------------------------

	return 0;
}

