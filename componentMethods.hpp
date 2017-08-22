#pragma once
//contains component implementations
#include "gridManip.hpp"


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

//get adjacent heat values
struct addAdjHeat{	
	static void func(function_args& tlocals, gsize_t adjAddress){
		if(tlocals.typegrid[adjAddress] == REACTOR_ID){
			tlocals.heat_g[tlocals.thisCell]+= static_cast<res_cell>(tlocals.locals_g[adjAddress].localB)/static_cast<res_cell>(tlocals.locals_g[adjAddress].localA);
		}
	}
};

//if adjacent component is part of a network, set this to be part of the same network
struct propagateNetwork{
	static void func(function_args& tlocals, gsize_t adjAddress){
		if(tlocals.typegrid[adjAddress] == SPREADER_ID){
			if(tlocals.locals_g[adjAddress].ptrA != nullptr){
				tlocals.locals_g[tlocals.thisCell].ptrA = tlocals.locals_g[adjAddress].ptrA;
			}
		}
	}
};

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

//-----Spreader methods-----
const void Spreader::component_setup(function_args& tlocals) noexcept{
	doForAdjacents<propagateNetwork>(tlocals);
	if(tlocals.locals_g[tlocals.thisCell].ptrA == 0){
		tlocals.locals_g[tlocals.thisCell].ptrA = tlocals.resNet.newNetwork();
	}
}
//assume that at this point, the network is created and working perfectly.
const void Spreader::component_action(function_args& tlocals) noexcept{
	struct spreaderAdjacentAction{
		static void func(function_args& tlocals, gsize_t adjAddress){
			auto type = tlocals.typegrid[adjAddress];
			auto providesHeat = static_properties[type].providesHeat;
			auto acceptsHeat = static_properties[type].acceptsHeat;
			auto thisPtrA = tlocals.locals_g[tlocals.thisCell].ptrA;
			auto otherPtrA = tlocals.locals_g[adjAddress].ptrA;
			
			if(providesHeat && (thisPtrA != otherPtrA)){
				//add heat to this network
				auto heatToAdd = static_cast<res_cell>(tlocals.locals_g[adjAddress].localB)/static_cast<res_cell>(tlocals.locals_g[adjAddress].localA);
				**static_cast<res_cell**>(tlocals.locals_g[tlocals.thisCell].ptrA)+=heatToAdd;
			}
			else if(acceptsHeat && (thisPtrA != otherPtrA)){
				//erk. need to fix somehow.
				auto heatToAdd = static_cast<res_cell>(tlocals.heat_g[adjAddress]);
				**static_cast<res_cell**>(tlocals.locals_g[tlocals.thisCell].ptrA)+=heatToAdd;
				tlocals.heat_g[adjAddress] = 0;
			}
		}
	};
	
	doForAdjacents<spreaderAdjacentAction>(tlocals);
	return;
}

//-----HeatSink methods-----
const void HeatSink::component_setup(function_args& tlocals) noexcept{
	Component::component_setup(tlocals);
	tlocals.heat_g[tlocals.thisCell] = HEATSINK_HEAT_START;
}
const void HeatSink::component_action(function_args& tlocals) noexcept{
	doForAdjacents<addAdjHeat>(tlocals);
	return;
}

//-----Reactor methods-----
const void Reactor::component_setup(function_args& tlocals) noexcept{
	
	Component::component_setup(tlocals);
	sumAdjacentComponents(tlocals);
	
	auto numAdjHeatAcceptors = DoForAllTypes<int,AdjacentAcceptHeat,function_args&,FoldAdd>(tlocals);
	tlocals.locals_g[tlocals.thisCell].localA = numAdjHeatAcceptors;
	
//		adjacency_t* this_adjacency = tlocals.adjacency_sg+(tlocals.thisCell*NUM_COMPONENT_TYPES);
//		auto numReactors = this_adjacency[REACTOR_ID]+1;
	tlocals.energy_g[tlocals.thisCell] = Reactor::HEAT_PRODUCED;
	//heat will be dispersed to adjacent components, so we store it in localB
	tlocals.locals_g[tlocals.thisCell].localB = Reactor::HEAT_PRODUCED;//numReactors*numReactors;
	//unless numAdjHeatAcceptors == 0, in which case we(attempt to) disqualify this design
	tlocals.heat_g[tlocals.thisCell] = (!numAdjHeatAcceptors)*Reactor::HEAT_PRODUCED;
	return;
}
const void Reactor::component_action(function_args& tlocals) noexcept{
	return;
}

//-----boiler methods-----
const void Boiler::component_setup(function_args& tlocals) noexcept{
	return;
}
const void Boiler::component_action(function_args& tlocals) noexcept{
	if adj is heatsink, subtract X energy.
	return;
}


//setup array



//return the score of the current grid. undefined if called before sim();
//modify this to change the goal of the program.
result_t scoreCurrentGrid(function_args& tlocals) noexcept{
	//need some way to check for things mid-sim?
	//ensure no heat buildup in a single element
	res_cell positiveHeat = 0;
	gsize_t numPositive = 0;
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		if(tlocals.heat_g[i]>0){
			numPositive++;
			positiveHeat+=tlocals.heat_g[i];
		}
	}
	//return negative average positive heat
	if(numPositive>0){
		return -static_cast<result_t>(positiveHeat/numPositive);
	}
	//ensure total heat is <=0
	res_cell heatSum = sum_grid<>(tlocals.heat_g, GRID_SIZE);
	if(heatSum > 0){
		return -static_cast<result_t>(heatSum);
	}
	int thisSum = sum_grid(tlocals.energy_g, GRID_SIZE);
	return static_cast<result_t>(thisSum);
}