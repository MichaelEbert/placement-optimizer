#pragma once
#include <limits>
#include <set>
//endpoint stuff
//heat flows from reactor->heat networks->endpoints.
//how to tell if component is part of heat network or endpoint? 
//Is it possible that the component can take heat from multiple networks, and if the heat is taken in ifrom the wrong network, it won't work? then it's endpoint.
//i think.


/*
Rationale:
Endpoints can be connected to multiple networks, and have to divide their resources up between the networks.
However, the only way to do this (that I know of) is to solve a systems of equations.
for example:
you have 2 networks, A and B
You have one endpoint E that can accept 4 units of resources.
E is connected to A and B.
then this is valid if the the resources in each network satisfy this system of equations:
A>4
B>4
A+B>4.

So we have to create a system of equations.
First, we make a list of all endpoints, represented as (attached networks,units accepted) pairs. This can be done asynchronously.
We then create the system of equations from this list. There is 1 equation for every network, additionally, there is one for every unique set of attached networks.


*/

//static const int MAX_ENDPOINTS = 16;

typedef void* endpoint_network_ptr;
//ensure that the endpoint_network_ptr is large enough
//static_assert(std::numeric_limits<endpoint_network_ptr>::max()>=MAX_NETWORKS,"endpoint_network_ptr not large enough to contain all networks. Please increase the size of endpoint_network_ptr.");


//networks adjacent to this endpoint
//typedef struct {
//	endpoint_network_ptr attachedNetwork[4];
//} EndpointNetworks;

class Endpoint {
	public:
	NetworkBitfield attachedNetworks;
	res_cell unitsAccepted;
	Endpoint(res_cell unitsAccepted, NetworkBitfield attachedNets){
		this->unitsAccepted = unitsAccepted;
		this->attachedNetworks = attachedNets;
	}
};

static void addEndpoint(std::vector<Endpoint>& endpoints, ResourceNetworkManager<res_cell>& resNet, res_cell unitsAccepted, float** firstNet, float** secondNet, float** thirdNet, float** fourthNet){
	//do some data cleaning:
	//dereference networks to actual networks
	//only include actual networks 
	NetworkBitfield newNets = 0;
	
	if(firstNet != nullptr) {
	newNets|=resNet.networkToBitfield(firstNet);}
	
	if(secondNet != nullptr) {
	newNets|=resNet.networkToBitfield(secondNet);}
	
	if(thirdNet != nullptr) {
	newNets|=resNet.networkToBitfield(thirdNet);}
	
	if(fourthNet != nullptr) {
	newNets|=resNet.networkToBitfield(fourthNet);}
	
	//determine if endpoint is already in:
	auto maybe = std::find_if(std::begin(endpoints),std::end(endpoints),[newNets](const Endpoint& e){return e.attachedNetworks == newNets;});
	//if not found, add. else add to unitsAccepted.
	if(maybe == endpoints.end()){
		endpoints.emplace_back(unitsAccepted, newNets);
	}
	else{
		maybe->unitsAccepted+=unitsAccepted;
	}
	return;
}

//an equation in the system of equations used to ensure a valid setup.
template<class R>
class EndpointEquation{
	public:
		R unitsAccepted = 0;
		NetworkBitfield networks;
		//verify that the target network values do not violate this equation
		bool verify(const ResourceNetworkManager<R>& resNet){
			R runningTotal = resNet.sumNetworkBitfield(networks);
			return (runningTotal + unitsAccepted) < 0;
		}
		EndpointEquation(Endpoint& targ, std::vector<Endpoint>& endpoints);
		EndpointEquation(NetworkBitfield singleTarg, std::vector<Endpoint>& endpoints);
	private:
	void addValidToAccepted(std::vector<Endpoint>& endpoints){
		for(auto e: endpoints){
				auto shouldAdd = networks & e.attachedNetworks;
				if(shouldAdd){
					unitsAccepted+=e.unitsAccepted;
				}
			}
	}
	
};

template<class R>
EndpointEquation<R>::EndpointEquation(Endpoint& targ, std::vector<Endpoint>& endpoints){
	networks = targ.attachedNetworks;
	addValidToAccepted(endpoints);
	return;
}

template<class R>
EndpointEquation<R>::EndpointEquation(NetworkBitfield singleTarg, std::vector<Endpoint>& endpoints){
	networks = singleTarg;
	addValidToAccepted(endpoints);
	return;
}

bool heatWorks(std::vector<Endpoint>& endpoints, ResourceNetworkManager<res_cell>& resNet){
	//for each boiler network combination:
	//1.total up each networks maximum heat
	//2.total up each combination of networks maximum heat

	std::vector<EndpointEquation<res_cell> > equations;
	equations.reserve(resNet.numActualNets + endpoints.size());
	auto actualNets = resNet.getValidNetworks();
	for(auto& net: actualNets){
		equations.emplace_back(resNet.networkToBitfield(net),endpoints);
	}
	
	for(auto& ep: endpoints){
		equations.emplace_back(ep,endpoints);
	}
	
	
	bool works = std::all_of(equations.begin(), equations.end(),[&resNet](EndpointEquation<res_cell>& a){return a.verify(resNet);});
	
	return works;
}
