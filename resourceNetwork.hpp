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
typedef int** Network;


const int MAX_NETWORKS = 16;
const int DIRECT_CONNECTED_NETWORKS = 4;//adjust for performance?

class ResourceNetwork{
	public:
		Network newNetwork();
		void joinNetworks(Network a,Network b);
	private:
		int curMaxNet = 0;
		int* networks[MAX_NETWORKS];
		std::vector<Network> valueNetworkLists(DIRECT_CONNECTED_NETWORKS)[MAX_NETWORKS];
		int values[MAX_NETWORKS];
		
};

Network ResourceNetwork::newNetwork(){
	networks[curMaxNet] = &values[curMaxNet];
	valueNetworkLists[curMaxNet].emplace_back(&networks[curMaxNet]);
	return &networks[curMaxNet++];
}

//join network B to network A
void ResourceNetwork::joinNetworks(Network a,Network b){
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
}


