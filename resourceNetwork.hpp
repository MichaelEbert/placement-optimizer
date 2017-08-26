#pragma once
#include <vector>
/*
this file implements resource networks -
some resources are better simulated as a single value spread across multiple components -
for example, heat. It can be spread around, and it only matters what the network total is.

implementing this with a bunch of pointers.
max_networks so we don't have to deal with mallocs and stuff.

testing to see if i can implement this better in a class.
*/

//need double pointer so we can move around the single pointers behind the scenes to point to different slots

const int MAX_NETWORKS = 16;
//const int DIRECT_CONNECTED_NETWORKS = 4;//adjust for performance? -need to actually create a constructor and use it first

//resource network for resource of type T
template<typename T>
using Network = T**;

template<typename T>
class ResourceNetworkManager{
	public:
		Network<T> newNetwork();
		void joinNetworks(Network<T> a,Network<T> b);
		void reset();
		T getVal(Network<T> a) const{
			return **a;
		}
		
	private:
		int curMaxNet = 0;
		int numActualNets = 0;
		T* networks[MAX_NETWORKS];
		std::vector<Network<T> > valueNetworkLists[MAX_NETWORKS];
		T values[MAX_NETWORKS];
};

template<typename T>
Network<T> ResourceNetworkManager<T>::newNetwork(){
	if(curMaxNet>= MAX_NETWORKS){
		printf("error max networks reached\n");
		return nullptr;
	}
	networks[curMaxNet] = &values[curMaxNet];
	valueNetworkLists[curMaxNet].emplace_back(&networks[curMaxNet]);
	numActualNets++;
	return &networks[curMaxNet++];
}

//join network B to network A
template<typename T>
void ResourceNetworkManager<T>::joinNetworks(Network<T> a,Network<T> b){
	if(*b==*a){
		return;
	}
	
	auto bValueIndex = (*b)-values;
	auto aValueIndex = (*a)-values;
	auto listA = valueNetworkLists[aValueIndex];
	auto listB = valueNetworkLists[bValueIndex];
	for(auto net:listB){
		*net = *a;
	}
	listA.insert(listA.end(),listB.begin(),listB.end());
	numActualNets--;
}

template<typename T>
void ResourceNetworkManager<T>::reset(){
	for(int i = 0; i < curMaxNet-1;i++){
		valueNetworkLists[i].clear();
	}
	curMaxNet=0;
	numActualNets=0;
	return;
}


