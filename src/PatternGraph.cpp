#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <time.h>
#include <pthread.h>

#include "../include/PatternGraph.h"

using namespace std;


PatternGraph::PatternGraph(int e,int n){
    //set the size
    edge=e;
    node=n;
    index_ptr_of_pattern=new int[n+1]();
    indices_of_pattern=new int[e*2]();
    num_of_neighbor=new int[n];
    order=new int[n];
    neighbor_restriction.resize(node);

    //init the array
    cout<<"input the index_ptr_of_pattern(from 0 to n)"<<endl;
    for(int i=0;i<node+1;i++){
        cin>>index_ptr_of_pattern[i];
    }

    cout<<"input the indices_of_pattern(from 0 to n)"<<endl;
    for(int i=0;i<edge*2;i++){
        cin>>indices_of_pattern[i];
    }

}
void PatternGraph::getTheNeighborOfEachNode(){
    for(int i=0;i<node;i++){
        num_of_neighbor[i]=index_ptr_of_pattern[i+1]-index_ptr_of_pattern[i];
    }
}
void PatternGraph::getTheMatchingOrder(){
    int max=node-1;
    int marker=0;
    while(marker!=node){
        for(int i=0;i<node;i++){
            if(max==num_of_neighbor[i]){
                order[marker]=i;
                marker++;
            }
        }
        max--;
    }
}

void PatternGraph::getNeighborRestriction(){
    for(int i=0;i<node;i++){
        for(int j=0;j<i;j++){//node before them in the order
            for(int k=index_ptr_of_pattern[order[i]];k<index_ptr_of_pattern[order[i]+1];k++){
                if(indices_of_pattern[k]==order[j]){
                    neighbor_restriction[order[i]].push_back(j);
                }
            }
        }
    }
}

void PatternGraph::clear(){
    if (index_ptr_of_pattern != nullptr) {
        delete[] index_ptr_of_pattern;
    }

    if (indices_of_pattern != nullptr) {
        delete[] indices_of_pattern;
    }

    if (num_of_neighbor != nullptr) {
        delete[] num_of_neighbor;
    }

    if (order != nullptr) {
        delete[] order;
    }
}

int PatternGraph::getEdge(){
    return edge;
};

int PatternGraph::getNode(){
    return node;
};

int * PatternGraph::getIndex_ptr_of_pattern(){
    return index_ptr_of_pattern;
};

int * PatternGraph::getIndices_of_pattern(){
    return indices_of_pattern;
};

int * PatternGraph::getOrder(){
    return order;
};

int * PatternGraph::getNum_of_neighbor(){
    return num_of_neighbor;
}

vector < vector<int> > PatternGraph::getNeighbor_restriction(){
    return neighbor_restriction;
};
