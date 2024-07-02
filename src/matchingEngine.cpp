#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <pthread.h>
#include <chrono>
#include "../include/matchingEngine.h"

using namespace std;
using namespace chrono;


matchingEngine::matchingEngine(int aNumber_of_thread,CSRGraph* aGraph,PatternGraph * aPatternGraph){
    number_of_thread=aNumber_of_thread;
    args=new ThreadData[aNumber_of_thread];
    graph=aGraph;
    patternGraph=aPatternGraph;
    number_of_node_for_last_matching=aGraph->getNode();
}

ThreadData * matchingEngine::get_the_data_prepared(int matching_round,int index_of_snapshot){

    int id=patternGraph->getOrder()[matching_round];
    //preparing data
    //query graph中的neighbor限制
    int size_of_neighbor_of_prenode=0;

    if(patternGraph->getNeighbor_restriction()[id].size()!=0){
        neighbor_of_prenode=new int[patternGraph->getNeighbor_restriction()[id].size()]();
        for(int p=0;p<patternGraph->getNeighbor_restriction()[id].size();p++){
            neighbor_of_prenode[p]=patternGraph->getNeighbor_restriction()[id][p];
        }
        size_of_neighbor_of_prenode=patternGraph->getNeighbor_restriction()[id].size();
    }

    //lunch the threads
    int full_node_for_each_thread=number_of_node_for_last_matching/number_of_thread;
    int remaining=number_of_node_for_last_matching-full_node_for_each_thread*number_of_thread;
    if(remaining==0){
        remaining=number_of_thread;
    } else {
        full_node_for_each_thread++;
    }
    int sharing_node_ptr=0;

    passing_node_to_thread_of_each=new int* [number_of_thread];
    dataPassingToThreads=new DataPassingToThreads * [number_of_thread];
    number_of_matching=new int[number_of_thread]();

    //prepare for the data
    for(int p=0;p<number_of_thread; p++){
        if(p<remaining){
            number_of_matching[p]=full_node_for_each_thread;
        }else {
            number_of_matching[p]=full_node_for_each_thread-1;
        }

        if(matching_round==0){
            passing_node_to_thread_of_each[p]=new int[number_of_matching[p]];
            for(int t=0;t<number_of_matching[p];t++){
                passing_node_to_thread_of_each[p][t]=graph->getTrue_index()[sharing_node_ptr];
                sharing_node_ptr++;
            }
        } else {
            passing_node_to_thread_of_each[p]=new int[number_of_matching[p]*matching_round];
            for(int t=0;t<number_of_matching[p];t++){
                for(int k=0;k<matching_round;k++){
                    passing_node_to_thread_of_each[p][t*matching_round+k]=node_of_matching[matching_round*sharing_node_ptr+k];
                }
                sharing_node_ptr++;
            }
        }

        dataPassingToThreads[p]=new DataPassingToThreads(passing_node_to_thread_of_each[p],matching_round,neighbor_of_prenode,size_of_neighbor_of_prenode,number_of_matching[p],index_of_snapshot,patternGraph->getNum_of_neighbor(),patternGraph->getOrder());

        args[p].data = dataPassingToThreads[p];
    }

    return args;
}

int matchingEngine::receiving_the_data(DataForPassingBack* ptr_get){
    int counter=0;
    node_of_matching.clear();

    for(int p=0;p<number_of_thread;p++){
        counter+=ptr_get[p].number_of_matching_node;

        node_of_matching.insert(node_of_matching.end(),ptr_get[p].matching_node.begin(),ptr_get[p].matching_node.end());
    }
    number_of_node_for_last_matching=counter;

    if(ptr_get!=nullptr){
        delete [] ptr_get;
    }

//    //testing
//    cout<<"the testing"<<endl;
//    for(int t=0;t<node_of_matching.size();t++){
//        cout<<node_of_matching[t]<<" ";
//    }
//    cout<<endl;

    return counter;
}

vector<int> matchingEngine::getNode_of_matching(){
    return node_of_matching;
}

void matchingEngine::Round_cleaner(int matching_round){
    int id=patternGraph->getOrder()[matching_round];

    for(int d=0;d<number_of_thread;d++){
        if(passing_node_to_thread_of_each[d]!=nullptr){
            delete [] passing_node_to_thread_of_each[d];
        }
    }

    if(patternGraph->getNeighbor_restriction()[id].size()!=0){
        delete [] neighbor_of_prenode;
    }

    if(number_of_matching!=nullptr){
        delete [] number_of_matching;
    }

}