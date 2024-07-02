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
    int *index_ptr_of_pattern; //the 
    int *indices_of_pattern;
    int *num_of_neighbor;
    int *order;
    vector < vector<int> > neighbor_restriction;

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


    void getTheNeighborOfEachNode(); //
    void getTheMatchingOrder();
    void getNeighborRestriction();
    void clear();
};

