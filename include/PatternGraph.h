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
    int edge;
    int node;
    int *index_ptr_of_pattern;
    int *indices_of_pattern;
    int *num_of_neighbor;
    int *order;
    vector < vector<int> > neighbor_restriction;

public:
    int getEdge();
    int getNode();
    int * getIndex_ptr_of_pattern();
    int * getIndices_of_pattern();
    int * getNum_of_neighbor();
    int * getOrder();
    vector < vector<int> > getNeighbor_restriction();

    PatternGraph(int e,int n);
    ~PatternGraph() { clear(); }
    void getTheNeighborOfEachNode();
    void getTheMatchingOrder();
    void getNeighborRestriction();
    void clear();
};

