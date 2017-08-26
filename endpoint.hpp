#pragma once
#include <limits>
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
	std::array<endpoint_network_ptr,4> attachedNetworks;
	res_cell unitsAccepted;
	 Endpoint(res_cell unitsAccepted, void* firstNet, void* secondNet, void* thirdNet, void* fourthNet){
		this->unitsAccepted = unitsAccepted;
		this->attachedNetworks[0] = firstNet;
		this->attachedNetworks[1] = secondNet;
		this->attachedNetworks[2] = thirdNet;
		this->attachedNetworks[3] = fourthNet;
	}
};



static void addEndpoint(std::vector<Endpoint>& endpoints, res_cell unitsAccepted, void* firstNet, void* secondNet, void* thirdNet, void* fourthNet){
	endpoints.emplace_back(unitsAccepted, firstNet, secondNet, thirdNet, fourthNet);
	return;
}

//part of the system of equations used to ensure a valid setup.
template<class R>
class EndpointEquation{
	public:
		R unitsAccepted = 0;
		std::vector<void*> networks; 
		//verify that the target network values do not violate this equation
		bool verify(const ResourceNetworkManager<R>& resNet){
			R runningTotal = 0;
			for(auto n: networks){
				runningTotal+=resNet.getVal(static_cast<res_cell**>(n));
			}
			return (runningTotal + unitsAccepted) < 0;
		}
		EndpointEquation(Endpoint& targ, std::vector<Endpoint>& endpoints);
		EndpointEquation(void* singleTarg, std::vector<Endpoint>& endpoints);
	private:
	addValidToAccepted(std::vector<Endpoint>& endpoints){
		for(auto e: endpoints){
				auto shouldAdd = std::find_first_of(std::begin(e.attachedNetworks),std::end(e.attachedNetworks),
				                                   std::begin(networks),std::end(networks));
				if(shouldAdd != e.attachedNetworks.end()){
					unitsAccepted+=e.unitsAccepted;
				}
			}
	}
	
};

template<class R>
EndpointEquation<R>::EndpointEquation(Endpoint& targ, std::vector<Endpoint>& endpoints){
	for(auto& n: targ.attachedNetworks){
		if(n != nullptr){
			networks.emplace_back(n);
		}
	}
	addValidToAccepted(endpoints);
	return;
}

template<class R>
EndpointEquation<R>::EndpointEquation(void* singleTarg, std::vector<Endpoint>& endpoints){
	networks.emplace_back(singleTarg);
	addValidToAccepted(endpoints);
	return;
}

bool heatWorks(std::vector<Endpoint>& endpoints, ResourceNetworkManager<res_cell>& resNet){
	//for each boiler network combination:
	//1.total up each networks maximum heat
	//2.total up each combination of networks maximum heat
//	unsigned short maxHeat[16];
//	//1:
//	//in SSE:
//	//LSHIFT,
//	//PBLENDVB,
//	//PADDW
//	for(int networkNum = 0; networkNum < 16; networkNum++){
//		unsigned short scratch = 0;
//		for(j in 1..16){
//			auto shouldCountThisBoiler = (bflags[j]<<networkNum);
//			if(shouldCountThisBoiler & 0x8000){//high bit set
//				scratch+=boilerHeat[j];
//			}
//		}
//		maxHeat[networkNum] = scratch;
//	}
//	//2:
//	for(auto refFlags: bflags){
//		for(int b = 0; b < 16; b++){
//			auto temp = bflags[b] & refFlags;
//			temp = (!temp)-1;//temp is now 0 if no refFlags, -1 if at least one refFlag was on
//			temp = temp & boilerHeat[b];
//		}
//		heat[refFlags] = temp;
//	}

	std::vector<EndpointEquation<res_cell> > equations;
	//TODO: single variable equations
	for(auto& ep: endpoints){
		equations.emplace_back(ep,endpoints);
	}
	
	
	bool works = std::all_of(equations.begin(), equations.end(),[resNet](EndpointEquation<res_cell>& a){return a.verify(resNet);});
	
	return works;
}
