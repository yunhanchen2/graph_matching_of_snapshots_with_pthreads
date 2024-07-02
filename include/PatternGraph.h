#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <pthread.h>

using namespace std;

class PatternGraph{
private:
    int edge; //the number of the current edges
    int node; //the number of the current nodes
    int *index_ptr_of_pattern; //the node array of the pattern graph
    int *indices_of_pattern; //the edge array of the pattern graph
    int *num_of_neighbor; //the degree of each node
    int *order; //the matching order
    vector < vector<int> > neighbor_restriction; //record the index of the neighbor for each node in the pattern graph

public:
    //get the private data
    int getEdge(); 
    int getNode();
    int * getIndex_ptr_of_pattern();
    int * getIndices_of_pattern();
    int * getNum_of_neighbor();
    int * getOrder();
    vector < vector<int> > getNeighbor_restriction();

    PatternGraph(int e,int n);
    ~PatternGraph() { clear(); }


    void getTheNeighborOfEachNode(); //get the num_of_neighbor from index_ptr_of_pattern & indices_if_pattern
    void getTheMatchingOrder();  //get the matching order of pattern graph 
    void getNeighborRestriction();  //get the index of the neighbor for each node in the pattern graph
    void clear();  //delete the data
};

