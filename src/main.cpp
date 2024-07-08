#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <pthread.h>
#include "../include/matchingEngine.h"
#include <chrono>

using namespace std;
using namespace chrono;

static int number_of_thread;

static CSRGraph graph;

pthread_mutex_t mu;

static vector<int> vectors_intersection(vector<int> v1,vector<int> v2){
    vector<int> v;
    sort(v1.begin(),v1.end());
    sort(v2.begin(),v2.end());
    set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v));//求交集
    return v;
}

void* graph_matching_threads(void *n){
    //in threads: get the neighbor and check degree store them in the vectors
    //record time

    ThreadData* dataT=(ThreadData*) n;
    DataPassingToThreads *dataPassingToThreads=dataT->data;

    DataForPassingBack *passingBack=new DataForPassingBack();

    passingBack->number_of_matching_node=0;

    int round=dataPassingToThreads->index_of_snapshot;

    passingBack->_old=0;
    passingBack->_new=0;

    if(dataPassingToThreads->round_index==0){
        //only check the degree
        for (int j = 0; j < dataPassingToThreads->number_of_matching; j++) {//满足其邻居条件以后,j为candidate node中的jth元素
            //get the neighbor
            vector <int> neighbor = graph.getTheNeighbor(dataPassingToThreads->passing_node_to_thread_of_each[j],round);

            if ( neighbor.size() >= dataPassingToThreads->num_of_neighbor[dataPassingToThreads->order[dataPassingToThreads->round_index]] ) {//degree也满足了
                passingBack->matching_node.push_back(dataPassingToThreads->passing_node_to_thread_of_each[j]);//存新match的
                passingBack->number_of_matching_node++;
                passingBack->_old++;
                passingBack->matching_weight.push_back(0);
            }
        }

    } else {
        int *tem;
        for(int i=0;i<dataPassingToThreads->number_of_matching;i++){
            int status=dataPassingToThreads->matching_weight_to_thread_of_each[i];

//            //testing
//            pthread_mutex_lock(&mu);
//            cout<<"the original status is: "<<status<<endl;
//            pthread_mutex_unlock(&mu);

            //each time pick one group
            tem=new int[dataPassingToThreads->round_index];
            for(int j=0;j<dataPassingToThreads->round_index;j++){
                tem[j]=dataPassingToThreads->passing_node_to_thread_of_each[i*dataPassingToThreads->round_index+j];
            }

            //get the neighbors
            vector<int> back;
            vector< vector<int> > neibor(dataPassingToThreads->size_of_neighbor_of_prenode_pattern);
            for(int k=0;k<dataPassingToThreads->size_of_neighbor_of_prenode_pattern;k++){//将邻居放入vector中
                vector <int> neighbor=graph.getTheNeighbor(tem[dataPassingToThreads->neighbor_of_prenode_pattern[k]],round);
                for(int r=0;r<neighbor.size();r++){
                    neibor[k].push_back(neighbor[r]);//放入的是对应的编号而非第几个
                }
//                //testing
//                pthread_mutex_lock(&mu);
//                for(int r=0;r<neighbor.size();r++){
//                    cout<<neighbor[r]<<" ";
//                }
//                cout<<endl;
//                pthread_mutex_unlock(&mu);

                //join the vector
                if(k==0){
                    back=neibor[0];
                } else {
                    back= vectors_intersection(back,neibor[k]);
                }
            }

            //cut off the node before the node and have a new matching
            vector<int>::iterator it;
            for(it=back.begin();it!=back.end();){
                bool check=true;
                for(int j=0;j<dataPassingToThreads->round_index;j++){
                    if(*it==tem[j]){
                        it=back.erase(it);
                        check= false;
                    }
                }
                if(check){
                    it++;
                }
            }

            //check the status
            for(int j=0;j<back.size();j++){
                for(int k=0;k<dataPassingToThreads->size_of_neighbor_of_prenode_pattern;k++){
                    if(status!=1){
                        status=graph.check_adding_status(tem[dataPassingToThreads->neighbor_of_prenode_pattern[k]],back[j],round);

//                        //testing
//                        pthread_mutex_lock(&mu);
//                        cout<<"the status of "<<tem[dataPassingToThreads->neighbor_of_prenode_pattern[k]]<<" and "<<back[j]<<"is: "<<status<<endl;
//                        pthread_mutex_unlock(&mu);


                    } else {
                        break;
                    }
                }

//                pthread_mutex_lock(&mu);
//                cout<<"the final status is: "<<status<<endl;
//                pthread_mutex_unlock(&mu);

                if(status==1){
                    passingBack->_new++;
                    passingBack->matching_weight.push_back(1);
                } else {
                    passingBack->_old++;
                    passingBack->matching_weight.push_back(0);
                }
            }

            //check the degree
            for (int j = 0; j < back.size(); j++) {//满足其邻居条件以后,j为candidate node中的jth元素
                if (graph.getTheNeighbor(back[j],round).size() >= dataPassingToThreads->num_of_neighbor[dataPassingToThreads->order[dataPassingToThreads->round_index]]) {//degree也满足了
                    for (int k = 0; k < dataPassingToThreads->round_index; k++) {
                        passingBack->matching_node.push_back(tem[k]);//将原来的存回去

                    }
                    passingBack->matching_node.push_back(back[j]);//存新match的
                    passingBack->number_of_matching_node++;
                }
            }

            delete [] tem;
        }
    }


//    //testing
//    pthread_mutex_lock(&mu);
//    cout<<"the number of old is: "<<passingBack->_old<<" and the number of new is: "<<passingBack->_new<<endl;
//    pthread_mutex_unlock(&mu);

    pthread_exit(passingBack);
}




int main(int argc,char* argv[]) {

    pthread_mutex_init(&mu, NULL);

    if (argc > 2) {
        number_of_thread=atoi(argv[2]);
        //get the pattern graph
        int e;
        int nod;

        cout<<"input the number of edge and node pattern graph"<<endl;
        cin>>e>>nod;

        PatternGraph patternGraph(e,nod);

        //get neighbor of each node
        patternGraph.getTheNeighborOfEachNode();

        //get the order
        patternGraph.getTheMatchingOrder();

        //find out the restriction of nodes
        patternGraph.getNeighborRestriction();

        //get the data graph
        char *pathname = argv[1];

        graph.loadTheGraph(pathname);//read+sort

        //do the matching
        cout<<"Input the number of the snapshots and the original ratio: "<<endl;
        int num_ss, ratio;
        cin>>num_ss>>ratio;
        graph.Generate_Snapshots(num_ss,ratio);

        pthread_t tid[number_of_thread];

        int counter=0;

        for(int r=0;r<graph.getTheNumberOfSnapshots();r++){
            //record time
            auto start = system_clock::now();

            matchingEngine engine(number_of_thread,&graph,&patternGraph);

            for(int i=0;i<patternGraph.getNode();i++){

                ThreadData * args;
                args=engine.get_the_data_prepared(i,r);


                for(int p=0;p<number_of_thread;p++){
                    pthread_create(&tid[p], NULL, graph_matching_threads, &args[p]);
                }


                //get vectors in each thread and merge them together
                DataForPassingBack* ptr_get=new DataForPassingBack[number_of_thread];
                void ** ptr=new void * [number_of_thread];

                for (int p = 0; p < number_of_thread; p++) {
                    pthread_join(tid[p], &(ptr[p]));
                    ptr_get[p]=*((DataForPassingBack*) (ptr[p]));
                }

                counter=engine.receiving_the_data(ptr_get,i);

                delete [] ptr;

                engine.Round_cleaner(i);
            }

            auto end = system_clock::now();
            auto duration= duration_cast<microseconds>(end-start);
            cout<<"time of matching for the snapshot#"<<r<<" is: "<<double (duration.count())*microseconds ::period ::num/microseconds::period::den<<endl;

            set < set<int> > ss;
            for(int i=0;i<counter;i++){
                set<int> each;
                for(int j=0;j<patternGraph.getNode();j++){
                    each.insert(engine.getNode_of_matching()[i*patternGraph.getNode()+j]);
                }
                ss.insert(each);
            }

            double ratio=engine.get_the_duplicate_ratio();
            cout<<"total counting for the snapshot#"<<r<<" is: "<<ss.size()<<endl;
            cout<<"duplicate ratio for the snapshot#"<<r<<" is: "<<ratio<<endl;
        }

    }

    pthread_mutex_destroy(&mu);


    return 0;
}
