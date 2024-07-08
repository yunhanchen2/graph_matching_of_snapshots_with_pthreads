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
    CSRGraph * graph;
    int * num_of_neighbor;
    int * order;
    int * passing_node_to_thread_of_each;
    int round_index;
    int *neighbor_of_prenode_pattern;
    int size_of_neighbor_of_prenode_pattern;
    int number_of_matching;
    int index_of_snapshot;
    int * matching_weight_to_thread_of_each;
    DataPassingToThreads(int *aPassing_node_to_thread_of_each,int aRound_index,int * aNeighbor_of_prenode_pattern,int aSize_of_neighbor_of_prenode_pattern,int aNumber_of_matching,int aIndex_of_snapshot,int * aNum_of_neighbor,int * aOrder,int * aMatching_weight_to_thread_of_each){
        passing_node_to_thread_of_each=aPassing_node_to_thread_of_each;
        round_index=aRound_index;
        neighbor_of_prenode_pattern=aNeighbor_of_prenode_pattern;
        size_of_neighbor_of_prenode_pattern=aSize_of_neighbor_of_prenode_pattern;
        number_of_matching=aNumber_of_matching;
        index_of_snapshot=aIndex_of_snapshot;
        num_of_neighbor=aNum_of_neighbor;
        order=aOrder;
        matching_weight_to_thread_of_each=aMatching_weight_to_thread_of_each;
    }
};


struct DataForPassingBack{
    int number_of_matching_node;
    vector <int> matching_node;
    vector <int> matching_weight;
    int _old;
    int _new;
};

struct ThreadData{
    DataPassingToThreads *data;
};



class matchingEngine{

private:
    vector<int> node_of_matching;
    vector <int> weight_of_matching;
    ThreadData *args;
    int number_of_thread;
    int number_of_node_for_last_matching;
    CSRGraph *graph;
    PatternGraph * patternGraph;

    int * _old_counter;
    int * _new_counter;

    //data used in the function of prepare for the data
    int **passing_node_to_thread_of_each;
    int **passing_weight_to_thread_of_each;
    DataPassingToThreads **dataPassingToThreads;
    int *number_of_matching;
    int* neighbor_of_prenode;

public:
    matchingEngine(int aNumber_of_thread,CSRGraph* aGraph,PatternGraph * aPatternGraph);
    ~matchingEngine(){delete [] _old_counter; delete [] _new_counter;}
    ThreadData * get_the_data_prepared(int matching_round,int index_of_snapshot);
    int receiving_the_data(DataForPassingBack* ptr_get,int round_index);
    vector<int> getNode_of_matching();
    void Round_cleaner(int matching_round);
    double get_the_duplicate_ratio();
};


