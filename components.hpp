#pragma once
#include <cstdio>
#include "gridManip.hpp"
//types.h - component types


//brute force search
//this won't work for real- 4 types 20 slots in 1 hour requires 300M tests/sec.
//no premature optimization! just do everything serial right now.
//but as a start, it'll work.


//switch between component setups with functor array, populated by fctr_arr[COMP_TYPE_ID] = COMP_TYPE::fctr;

//best is probably to only write to your cell if possible. more parallel that way.
//reserve space

//need to pass these in as vars :(
//static adjacency_t adjacency1_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
//static res_cell energy1_g[GRID_SIZE];
//static res_cell heat1_g[GRID_SIZE];
//static res_cell locala1_g[GRID_SIZE];
//static cell_properties properties1_g[GRID_SIZE];

/*want: a grid type that I can use interchangably, regardless of teh size of the cells.
-templates or passing around array size
*/

//another function for 

//for REACTOR
//types:
//NONE
//REACTOR
//HEATSINK
//SPREADER
enum component_ids{
	NONE_ID,
	REACTOR_ID,
	HEATSINK_ID,
	SPREADER_ID
};

enum resource_ids{
	RES_ENERGY_ID,
	RES_HEAT_ID
};

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

//get adjacent heat values
struct getAdjHeat{	
	static void func(function_args& tlocals, gsize_t adjAddress){
		if(tlocals.typegrid[adjAddress] == REACTOR_ID){
			tlocals.heat_g[tlocals.thisCell]+= (tlocals.locals_g[adjAddress].localB)/(tlocals.locals_g[adjAddress].localA);
		}
	}
};


class Component{
public:
	static const bool acceptsHeat = true;
	static const bool providesHeat = false;
	static const void component_setup(function_args& tlocals) noexcept{
		sumAdjacentComponents(tlocals);
		(tlocals.properties_g)[tlocals.thisCell].acceptsHeat = acceptsHeat;
		return;
	}
	static const void component_action(function_args& tlocals) noexcept{
		return;
	}
	
};

class None : public Component{
	public:
		static const int index = NONE_ID;
		static const bool acceptsHeat = false;
		static const bool providesHeat = false;
		static const void component_setup(function_args& tlocals) noexcept{
			return;
		}
};

class Spreader : public Component{
public:
	static const int index = SPREADER_ID;
	static const bool acceptsHeat = true;
	static const bool providesHeat = true;
	static const void component_action(function_args& tlocals) noexcept{
		doForAdjacents<getAdjHeat>(tlocals);
		return;
	}
	
};

/*
how heat sinks work:
setup: calculate heat produced
run: attach to heat networks?
*/
class HeatSink: public Component{
public:
	static const int index = HEATSINK_ID;
	static const bool acceptsHeat = true;
	static const bool providesHeat = false;
	static const res_cell HEATSINK_HEAT_START = -5;
	static const void component_setup(function_args& tlocals) noexcept{
		Component::component_setup(tlocals);
		tlocals.heat_g[tlocals.thisCell] = HEATSINK_HEAT_START;
	}
	static const void component_action(function_args& tlocals) noexcept{
		doForAdjacents<getAdjHeat>(tlocals);
		return;
	}
};

/*
how reactors work:
setup: calculate heat/energy produced, get adjacent heat acceptors (for splitting heat)
run: nothing
*/
class Reactor: public Component{
	public:
	static const int index = REACTOR_ID;
	static const bool acceptsHeat = false;
	static const bool providesHeat = true;
//	static constexpr Setup()
//	adjacency REACTOR(){
//		this.ticks++;
//	}
//	adjacency DEFAULT(){
//		adjComponents+= (adj.acceptsHeat)
//	}
//	adjEffect<T::acceptsHeat,variables::adjComponents,operator+>
	
	static const void component_setup(function_args& tlocals) noexcept{
		Component::component_setup(tlocals);
		auto numAdjHeatAcceptors = FoldOverAllTypes<int,AdjacentAcceptHeat,function_args&,FoldAdd>(tlocals);
		tlocals.locals_g[tlocals.thisCell].localA = numAdjHeatAcceptors;
		adjacency_t* this_adjacency = tlocals.adjacency_sg+(tlocals.thisCell*NUM_COMPONENT_TYPES);
		auto numReactors = this_adjacency[REACTOR_ID]+1;
		tlocals.energy_g[tlocals.thisCell] = numReactors;
		//heat will be dispersed to adjacent components, so we store it in localB
		tlocals.locals_g[tlocals.thisCell].localB = numReactors*numReactors;
		//unless numAdjHeatAcceptors == 0, in which case we(attempt to) disqualify this design
		tlocals.heat_g[tlocals.thisCell] = (!numAdjHeatAcceptors)*120;
		
	}
	static const void component_action(function_args& tlocals) noexcept{
		return;
	}
};

//setup array

const component_func_t component_setup_arr[] = {
	None::component_setup,
	Reactor::component_setup,
	HeatSink::component_setup,
	Spreader::component_setup	
};

const component_func_t component_action_arr[] = {
	None::component_action,
	Reactor::component_action,
	HeatSink::component_action,
	Spreader::component_action	
};

template <typename T,size_t N>
constexpr size_t array_size(T(&)[N]){
	return N;
}

//return the score of the current grid. undefined if called before sim();
//modify this to change the goal of the program.
result_t scoreCurrentGrid(function_args& tlocals) noexcept{
		res_cell heatSum = sum_grid<>(tlocals.heat_g, GRID_SIZE);
		if(heatSum > 0){
			return -static_cast<result_t>(heatSum);
		}
		int thisSum = sum_grid(tlocals.energy_g, GRID_SIZE);
		return static_cast<result_t>(thisSum);
}
	

//---attempt to do generic property listing/selection---
//template<class c>
//get_class_index(){
//	return c::index;
//}
//
//sum_adj_with_prop(prop p){
//	if(c::p){
//		n+=adj[c::index]
//	}
//	
//}
////prop ACCEPTS_HEAT
//sum_adj_with_prop(){
//	return adj[HeatSink::index]+adj[Spreader::index];
//}
//
////recursive template?
//template<class c, classes cs>
//sum_adj_recursive(){
//	return (maybe c::index)+sum_adj_recursive<cs>();
//}
//---attempt end---

//generic

//spreader has no setup

//generate property set function(?)

//function set ???


//and emit boilerplate code for all the types of components possible. -- so templates.
//don't think I can generate more code with templates - just genericize it. so would need separate frontend to parse
//before compiling main app.
//and maybe set fields to arrays? --so overload operator->.
//in the template, need to access a const variable. -- std::enable_if
//aargh. JUST HARDCODE IT FIRST, THEN EXTEND!.
/*
 * ex:
 * simulating a farm. want to know how much space I might need in a year.
 * it would be really nice if I could just 
 * struct cow{
 * int legs = 4;
 * int number_in_herd;
 * const bool can_multiply=true;
 * }
 * struct wagon{
 * int legs;
 * int num_in_herd;
 * const bool can_multiply = false;
 * 
 * */
 
 //next challenge: enumeration of something with templates
 //goal: initialize functor array, each element of array is from <comp_type>::FCTR
