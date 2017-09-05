#pragma once
#include <vector>
#include "fastlist.hpp"
/*
this file implements resource networks -
some resources are better simulated as a single value spread across multiple components -
for example, heat. It can be spread around, and it only matters what the network total is.

implementing this with a bunch of pointers.
max_networks so we don't have to deal with mallocs and stuff.

testing to see if i can implement this better in a class.
*/

//need double pointer so we can move around the single pointers behind the scenes to point to different slots


//vector<> code takes an absurd amount of time to run, so I get to roll my own.



const int MAX_NETWORKS = 16;

//use a bitfield for certain network operations as it's optimal for sorting and comparisons.
typedef uint32_t NetworkBitfield;
static_assert(MAX_NETWORKS<sizeof(NetworkBitfield)*8,"NetworkBitfield not large enough to store all networks.Decrease maximum networks or increase NetworkBitfield size.\n");
//const int DIRECT_CONNECTED_NETWORKS = 4;//adjust for performance? -need to actually create a constructor and use it first


//resource network for resource of type T
template<typename T>
using Network = T**;

template<typename T>
class ResourceNetworkManager{
	public:
		static bool areSameNetwork(Network<T> a, Network<T> b){
			return (a != nullptr) && (b != nullptr) && *a == *b;
		}
		
		Network<T> newNetwork();
		void joinNetworks(Network<T> a,Network<T> b);
		void reset();
		T getVal(Network<T> a) const{
			return **a;
		}
		int numActualNets = 0;
		std::vector<Network<T> > getValidNetworks();
		NetworkBitfield networkToBitfield(Network<T> a){
			return 1<<(*a-values);
		}
		
		T sumNetworkBitfield(NetworkBitfield networks) const;
		void printDebugInfo() const;
		void setDebug(){rnDebug = true;}
		void clearDebug(){rnDebug = false;}
	private:
		bool rnDebug = false;
		int nextNet = 0;
		T* networks[MAX_NETWORKS];
		std::vector<Network<T> > valueNetworkLists[MAX_NETWORKS];
		T values[MAX_NETWORKS];
};

template<typename T>
Network<T> ResourceNetworkManager<T>::newNetwork(){
	if(nextNet>= MAX_NETWORKS){
		printf("error max networks reached\n");
		return nullptr;
	}
	networks[nextNet] = &values[nextNet];
	if(rnDebug){
		printf("nextNet is %d\n",nextNet);
		printf("assigning to %p value: %p\n",&networks[nextNet],&values[nextNet]);
		printf("valNetList has %d elements\n",valueNetworkLists[nextNet].size());
	}
	valueNetworkLists[nextNet].emplace_back(&networks[nextNet]);
	numActualNets++;

	return &networks[nextNet++];
}

//join network B to network A. b or a can be null (is better to check for null here or before call?)
template<typename T>
void ResourceNetworkManager<T>::joinNetworks(Network<T> a,Network<T> b){
	if(a == nullptr || b == nullptr || *b==*a){
		return;
	}
	if(rnDebug){
		printf("joining %p and %p\n",a,b);
	}
	
	auto bValueIndex = (*b)-values;
	auto aValueIndex = (*a)-values;
	auto& listA = valueNetworkLists[aValueIndex];
	auto& listB = valueNetworkLists[bValueIndex];
	for(auto net:listB){
		*net = *a;
	}
	/*listA.addAll(listB);*/listA.insert(listA.end(),listB.begin(),listB.end());
	numActualNets--;
}

//reinitialize for new map
template<typename T>
void ResourceNetworkManager<T>::reset(){
	for(int i = 0; i < nextNet;i++){
		valueNetworkLists[i].clear();
	}
	for(int i = 0; i < MAX_NETWORKS; i++){
		values[i] = 0;
	}
	nextNet=0;
	numActualNets=0;
	return;
}

//need to get list of all valid networks.
template<typename T>
std::vector<Network<T> > ResourceNetworkManager<T>::getValidNetworks(){
	std::vector<Network<T> > temp;
	temp.reserve(numActualNets);
	for(int i = 0; i < nextNet; i++){
		if(networks[i] == &values[i]){
			temp.emplace_back(&(networks[i]));
		}
	}
	return temp;
}

//get the sum of the networks represented in the bitfield
template<typename T>
T ResourceNetworkManager<T>::sumNetworkBitfield(NetworkBitfield networks) const{
	T sum = 0;
	for(int i = 0; i < nextNet; i++){
		if(networks & 1<<i){
			sum += values[i];
		}
	}
	return sum;
}

template<typename T>
void ResourceNetworkManager<T>::printDebugInfo() const{
	printf("MaxNetsUsed: %d\nValsUsed: %d\n",nextNet,numActualNets);
	printf("#:Netwk                 PtsTo                        valAdr                value\n");
	for(int i = 0;  i < nextNet; i++){
		printf("%d:%p\t%p\t\t%p\t%d\n",i,&networks[i], networks[i], &values[i], values[i]);
	}
}

template<>
void ResourceNetworkManager<float>::printDebugInfo() const{
	printf("MaxNetsUsed: %d\nValsUsed: %d\n",nextNet,numActualNets);
	printf("#:Netwk                 PtsTo                        valAdr                value\n");
	for(int i = 0;  i < nextNet; i++){
		printf("%d:%p\t%p\t\t%p\t%f\n",i,&networks[i], networks[i], &values[i], values[i]);
	}
}

