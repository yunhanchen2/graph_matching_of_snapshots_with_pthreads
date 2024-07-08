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

matchingEngine::matchingEngine(int aNumber_of_thread, CSRGraph * aGraph, PatternGraph * aPatternGraph){
    number_of_thread=aNumber_of_thread;
    args=new ThreadData[aNumber_of_thread];
    graph=aGraph;
    patternGraph=aPatternGraph;
    number_of_node_for_last_matching=aGraph->getNode();
    _old_counter=new int[aPatternGraph->getNode()];
    _new_counter=new int[aPatternGraph->getNode()];
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
    passing_weight_to_thread_of_each=new int* [number_of_thread];
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
            passing_weight_to_thread_of_each[p]=new int[number_of_matching[p]];
            for(int t=0;t<number_of_matching[p];t++){
                passing_node_to_thread_of_each[p][t]=graph->getTrue_index()[sharing_node_ptr];
                passing_weight_to_thread_of_each[p][t]=0;
                sharing_node_ptr++;
            }

        } else {
            passing_node_to_thread_of_each[p]=new int[number_of_matching[p]*matching_round];
            passing_weight_to_thread_of_each[p]=new int[number_of_matching[p]];
            for(int t=0;t<number_of_matching[p];t++){
                for(int k=0;k<matching_round;k++){
                    passing_node_to_thread_of_each[p][t*matching_round+k]=node_of_matching[matching_round*sharing_node_ptr+k];
                }
                passing_weight_to_thread_of_each[p][t]=weight_of_matching[sharing_node_ptr];
                sharing_node_ptr++;
            }
        }

        dataPassingToThreads[p]=new DataPassingToThreads(passing_node_to_thread_of_each[p],matching_round,neighbor_of_prenode,size_of_neighbor_of_prenode,number_of_matching[p],index_of_snapshot,patternGraph->getNum_of_neighbor(),patternGraph->getOrder(),passing_weight_to_thread_of_each[p]);


        args[p].data = dataPassingToThreads[p];

    }

    return args;
}

int matchingEngine::receiving_the_data(DataForPassingBack* ptr_get,int round_index){
    int counter=0;

    int _new_acc=0;
    int _old_acc=0;

    node_of_matching.clear();
    weight_of_matching.clear();

    for(int p=0;p<number_of_thread;p++){
        counter+=ptr_get[p].number_of_matching_node;
        _old_acc+=ptr_get[p]._old;
        _new_acc+=ptr_get[p]._new;

        node_of_matching.insert(node_of_matching.end(),ptr_get[p].matching_node.begin(),ptr_get[p].matching_node.end());
        weight_of_matching.insert(weight_of_matching.end(),ptr_get[p].matching_weight.begin(),ptr_get[p].matching_weight.end());
    }
    number_of_node_for_last_matching=counter;

    if(ptr_get!=nullptr){
        delete [] ptr_get;
    }

//    //testing
//    cout<<"the matching of node: "<<endl;
//    for(int t=0;t<node_of_matching.size();t++){
//        cout<<node_of_matching[t]<<" ";
//    }
//    cout<<endl;

    _old_counter[round_index]=_old_acc;
    _new_counter[round_index]=_new_acc;


//    //testing
//    cout<<"the matching weight for round#"<<round_index<<": "<<endl;
//    cout<<"for the new is: "<<_new_acc<<" and the old is: "<<_old_acc<<endl;
//    cout<<"the matching weight:"<<endl;
//    for(int i=0;i<weight_of_matching.size();i++){
//        cout<<weight_of_matching[i]<<" ";
//    }
//    cout<<endl;
//    cout<<"the number of last matching: "<<counter<<endl;
//

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

    for(int d=0;d<number_of_thread;d++){
        delete [] passing_weight_to_thread_of_each[d];
    }

    if(patternGraph->getNeighbor_restriction()[id].size()!=0){
        delete [] neighbor_of_prenode;
    }

    if(number_of_matching!=nullptr){
        delete [] number_of_matching;
    }

}

double matchingEngine::get_the_duplicate_ratio(){
    double ratio;
    double _new_total=0;
    double _old_total=0;
    for(int i=0;i<number_of_thread;i++){
        _new_total+=_new_counter[i];
        _old_total+=_old_counter[i];
    }

//    cout<<"the total new is "<<_new_total<<" and the total old is "<<_old_total<<endl;

    if(_new_total==0){
        return 0;
    } else {
        ratio=_new_total/(_new_total+_old_total);
        return ratio;
    }
}
