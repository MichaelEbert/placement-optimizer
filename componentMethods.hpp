#pragma once
//contains component implementations
#include "gridManip.hpp"
#include "doForTypes.hpp"
#include "printMatrix.hpp"


//counts the number of adjacent components that can accept heat
template<typename ComponentType>
struct AdjacentAcceptHeat{
	static int func(function_args& tlocals){
		if(ComponentType::acceptsHeat){
			adjacency_t* this_adjacency = tlocals.adjacency_sg+(tlocals.thisCell*NUM_COMPONENT_TYPES);
			return this_adjacency[ComponentType::index];
		}
		return 0;
	}
};

//does adjacent provide heat?
template<typename ComponentType>
struct doProvideHeat{
	static int func(function_args& tlocals){
		if(ComponentType::providesHeat){
			//adjacency_t* this_adjacency = tlocals.adjacency_sg+(tlocals.thisCell*NUM_COMPONENT_TYPES);
			return ComponentType::index;
		}
		return 0;
	}
};

////get adjacent heat values
//struct addAdjHeat{	
//	static void func(function_args& tlocals, gsize_t adjAddress){
//		if(tlocals.typegrid[adjAddress] == REACTOR_ID){
//			tlocals.heat_g[tlocals.thisCell]+= static_cast<res_cell>(tlocals.locals_g[adjAddress].localB)/static_cast<res_cell>(tlocals.locals_g[adjAddress].localA);
//		}
//	}
//};



//-----Component methods-----
const void Component::component_setup(function_args& tlocals) noexcept{
	//(tlocals.properties_g)[tlocals.thisCell].acceptsHeat = acceptsHeat;
	return;
}
const void Component::component_action(function_args& tlocals) noexcept{
	return;
}

//-----None methods-----
const void None::component_setup(function_args& tlocals) noexcept{
	return;
}

//-----Reactor methods-----
const void Reactor::component_setup(function_args& tlocals) noexcept {
	struct setAdjHeatAcceptors {
		static void func(function_args& tlocals, gsize_t adjAddress) {
			//if accepts heat, add 1
			if (static_properties[tlocals.typegrid[adjAddress]].acceptsHeat) {
				tlocals.locals_g[tlocals.thisCell].localA++;
			}
			return;
		}
	};

	//Component::component_setup(tlocals);
	doForAdjacents<setAdjHeatAcceptors>(tlocals);

	auto numAdjHeatAcceptors = tlocals.locals_g[tlocals.thisCell].localA;

	tlocals.energy_g[tlocals.thisCell] = Reactor::heatProduced;
	//heat will be dispersed to adjacent components, so we store it in localB
	tlocals.locals_g[tlocals.thisCell].localB = Reactor::heatProduced;
	//unless numAdjHeatAcceptors == 0, in which case we(attempt to) disqualify this design
	tlocals.heat_g[tlocals.thisCell] = (!numAdjHeatAcceptors)*Reactor::heatProduced;
	return;
}
const void Reactor::component_action(function_args& tlocals) noexcept {
	return;
}

//-----Spreader methods-----
const void Spreader::component_setup(function_args& tlocals) noexcept{
	//if adjacent component ACCEPTS heat and is part of a network, set this to be part of the same network.
	//if we encounter 2 adjacent networks, join them.
	struct spreaderAcquireNetwork{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto& thisLocals = tlocals.locals_g[tlocals.thisCell];
			auto& thatLocals = tlocals.locals_g[adjAddress];
			if(static_properties[tlocals.typegrid[adjAddress]].acceptsHeat && thatLocals.netPtr != nullptr){
				//todo: check if doing this is faster than if(this)then{join(this,that)}else{this=that}
				tlocals.resNet.joinNetworks(static_cast<float**>(thisLocals.netPtr),static_cast<float**>(thatLocals.netPtr));
				thisLocals.netPtr = thatLocals.netPtr;
			}
		}
	};
	doForAdjacents<spreaderAcquireNetwork>(tlocals);
	if(tlocals.locals_g[tlocals.thisCell].netPtr == 0){
		tlocals.locals_g[tlocals.thisCell].netPtr = tlocals.resNet.newNetwork();
	}
}
//assume that at this point, the network is created and working perfectly.
const void Spreader::component_action(function_args& tlocals) noexcept{
	struct spreaderAdjacentAction{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto type = tlocals.typegrid[adjAddress];
			auto providesHeat = static_properties[type].providesHeat;
			auto acceptsHeat = static_properties[type].acceptsHeat;
			auto thisNet = tlocals.locals_g[tlocals.thisCell].netPtr;
			auto otherNet = tlocals.locals_g[adjAddress].netPtr;
			
			if(providesHeat && (!tlocals.resNet.areSameNetwork(thisNet,otherNet))){
				//add heat to this network
				auto heatToAdd = static_cast<res_cell>(tlocals.locals_g[adjAddress].localB)/static_cast<res_cell>(tlocals.locals_g[adjAddress].localA);
				**(tlocals.locals_g[tlocals.thisCell].netPtr)+=heatToAdd;
			}
			else if(acceptsHeat && !tlocals.resNet.areSameNetwork(thisNet,otherNet)){
				//is this even possible?? TODO fix
//				auto heatToAdd = static_cast<res_cell>(tlocals.heat_g[adjAddress]);
//				**(tlocals.locals_g[tlocals.thisCell].netPtr)+=heatToAdd;
//				tlocals.heat_g[adjAddress] = 0;
			}
		}
	};
	
	doForAdjacents<spreaderAdjacentAction>(tlocals);
	return;
}

//-----HeatSink methods-----
const void HeatSink::component_setup(function_args& tlocals) noexcept{
	//if adj component PROVIDES heat and is part of a network, add this to that network.
	//join networks if needed.
	struct sinkAcquireNetwork{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto& thisLocals = tlocals.locals_g[tlocals.thisCell];
			auto& thatLocals = tlocals.locals_g[adjAddress];
			
			if(static_properties[tlocals.typegrid[adjAddress]].providesHeat && thatLocals.netPtr != nullptr){
				tlocals.resNet.joinNetworks(static_cast<float**>(thisLocals.netPtr),static_cast<float**>(thatLocals.netPtr));
				thisLocals.netPtr = thatLocals.netPtr;
			}
		}
	};
	
	//Component::component_setup(tlocals);
	doForAdjacents<sinkAcquireNetwork>(tlocals);
	if(tlocals.locals_g[tlocals.thisCell].netPtr == 0){
		tlocals.locals_g[tlocals.thisCell].netPtr = tlocals.resNet.newNetwork();
	}
	tlocals.heat_g[tlocals.thisCell] = HeatSink::heatProduced;
}
const void HeatSink::component_action(function_args& tlocals) noexcept{
	struct sinkAdjacentAction{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto type = tlocals.typegrid[adjAddress];
			auto providesHeat = static_properties[type].providesHeat;
			auto acceptsHeat = static_properties[type].acceptsHeat;
			auto thisnetPtr = tlocals.locals_g[tlocals.thisCell].netPtr;
			auto othernetPtr = tlocals.locals_g[adjAddress].netPtr;
			
			if(providesHeat && (!tlocals.resNet.areSameNetwork(thisnetPtr,othernetPtr))){
				//add heat to this network
				auto heatToAdd = static_cast<res_cell>(tlocals.locals_g[adjAddress].localB)/static_cast<res_cell>(tlocals.locals_g[adjAddress].localA);
				**(tlocals.locals_g[tlocals.thisCell].netPtr)+=heatToAdd;
			}
		}
	};
	doForAdjacents<sinkAdjacentAction>(tlocals);
	**(tlocals.locals_g[tlocals.thisCell].netPtr)+=HeatSink::heatProduced;
	return;
}


//fuck shit this is creating a systems of equations.
//-----boiler methods-----

const void Boiler::component_setup(function_args& tlocals) noexcept{
	//if boiler is next to a heat giver, thats bad.
	struct getAdjHeatProducers{
		static void func(function_args& tlocals, gsize_t adjAddress){
			if(static_properties[tlocals.typegrid[adjAddress]].providesHeat){
				tlocals.heat_g[tlocals.thisCell]+=Reactor::heatProduced;//hopefully negate this unit.
			}
		}
	};
	doForAdjacents<getAdjHeatProducers>(tlocals);
	
	return;
}
const void Boiler::component_action(function_args& tlocals) noexcept{
	//fuck fix this later
	struct getTwoPointerLocs{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto adjType = tlocals.typegrid[adjAddress];
			if(static_properties[adjType].networkable){
				auto& targetA = tlocals.locals_g[adjAddress].netPtr;
				if(targetA != nullptr){
					if(tlocals.locals_g[tlocals.thisCell].netPtr == 0){
						tlocals.locals_g[tlocals.thisCell].netPtr = targetA;
					}
					else{
						tlocals.locals_g[tlocals.thisCell].ptrB = targetA;
					}
				}
			}
			
			
		}
	};
	doForAdjacents<getTwoPointerLocs>(tlocals);
	auto mylocals = tlocals.locals_g[tlocals.thisCell];
	
	addEndpoint(tlocals.endpointList,tlocals.resNet,Boiler::heatProduced,static_cast<float**>(mylocals.netPtr),static_cast<float**>(mylocals.ptrB),nullptr,nullptr);
	//if adj is heatsink, subtract X energy.
	return;
}


//setup array



//return the score of the current grid. undefined if called before sim();
//modify this to change the goal of the program.
result_t scoreCurrentGrid(function_args& tlocals) noexcept{
	
	//need some way to check for things mid-sim?
	
	//TODO: remake some way to rank failing designs
	auto a = heatWorks(tlocals.endpointList,tlocals.resNet);
	if(a == false){
		return -1;
	}
//	//ensure no heat buildup in a single element
//	res_cell positiveHeat = 0;
//	gsize_t numPositive = 0;
//	for(gsize_t i = 0; i < GRID_SIZE; i++){
//		if(tlocals.heat_g[i]>0){
//			numPositive++;
//			positiveHeat+=tlocals.heat_g[i];
//		}
//	}
//	//return negative average positive heat
//	if(numPositive>0){
//		return -static_cast<result_t>(positiveHeat/numPositive);
//	}
//	
	//ensure total heat is <=0
	res_cell heatSum = sum_grid(tlocals.heat_g, GRID_SIZE);
	if(heatSum > 0){
		return -static_cast<result_t>(heatSum);
	}
	res_cell thisSum = sum_grid(tlocals.energy_g, GRID_SIZE);
	return static_cast<result_t>(thisSum);
}
