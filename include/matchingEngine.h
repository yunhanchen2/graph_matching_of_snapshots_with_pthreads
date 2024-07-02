#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <pthread.h>
#include <chrono>
#include "../include/PatternGraph.h"
#include "../include/CSRGraph.h"

using namespace std;
using namespace chrono;

class DataPassingToThreads{
public:
    int * num_of_neighbor;
    int * order;
    int * passing_node_to_thread_of_each;
    int round_index;
    int *neighbor_of_prenode_pattern;
    int size_of_neighbor_of_prenode_pattern;
    int number_of_matching;
    int index_of_snapshot;
    DataPassingToThreads(int *aPassing_node_to_thread_of_each,int aRound_index,int * aNeighbor_of_prenode_pattern,int aSize_of_neighbor_of_prenode_pattern,int aNumber_of_matching,int aIndex_of_snapshot,int * aNum_of_neighbor,int * aOrder){
        passing_node_to_thread_of_each=aPassing_node_to_thread_of_each;
        round_index=aRound_index;
        neighbor_of_prenode_pattern=aNeighbor_of_prenode_pattern;
        size_of_neighbor_of_prenode_pattern=aSize_of_neighbor_of_prenode_pattern;
        number_of_matching=aNumber_of_matching;
        index_of_snapshot=aIndex_of_snapshot;
        num_of_neighbor=aNum_of_neighbor;
        order=aOrder;
    }
};


struct DataForPassingBack{
    int number_of_matching_node;
    vector <int> matching_node;
};

struct ThreadData{
    DataPassingToThreads *data;
};



class matchingEngine{

private:
    vector<int> node_of_matching;  //record all the matching (n*m) m is the number of the node in pattern graph
    ThreadData *args; //the data collection of the data passing to threads
    int number_of_thread;  //number of threads to do the matching
    int number_of_node_for_last_matching; //the number of matching for the last round
    CSRGraph *graph;  //point to the CSRGraph object created in main
    PatternGraph * patternGraph;  //point to the PatternGraph object in main 

    //data used in the function of prepare for the data that passed to threads
    int **passing_node_to_thread_of_each;
    DataPassingToThreads **dataPassingToThreads;
    int *number_of_matching;
    int* neighbor_of_prenode;

public:
    //get the private data
    vector<int> getNode_of_matching();  

    matchingEngine(int aNumber_of_thread,CSRGraph* aGraph,PatternGraph * aPatternGraph); 
    ThreadData * get_the_data_prepared(int matching_round,int index_of_snapshot);  //get the data that passing to each threads
    int receiving_the_data(DataForPassingBack* ptr_get);  //receive the data that passing back from threads
    void Round_cleaner(int matching_round);  //delete the data
};


