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
static adjacency_t adjacency1_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
static res_cell energy1_g[GRID_SIZE];
static res_cell heat1_g[GRID_SIZE];
static res_cell locala1_g[GRID_SIZE];
static cell_properties properties1_g[GRID_SIZE];

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
//NEXT TO DO: replace cell* thisCell with a cell offset - can just go types[offset] to get this cell as variables are global.
//template so that we can use exactly the arguments we want?
class Component{
public:
	static const bool acceptsHeat = true;
	static const void component_setup(gsize_t thisCell, thread_grids& tgrids) noexcept{
		sumAdjacentComponents(thisCell, tgrids);
		(tgrids.properties_g)[thisCell].acceptsHeat = acceptsHeat;
		return;
	}
	static const void component_action(gsize_t thisCell, thread_grids& tgrids) noexcept{
		return;
	}
	
};

class None : public Component{
	public:
		static const void component_setup(gsize_t thisCell, thread_grids& tgrids) noexcept{
			return;
		}
};

class Spreader : public Component{
public:
	
};

class HeatSink: public Component{
public:
	static const signed int HEATSINK_HEAT_START = -5;
	static const void component_action(gsize_t thisCell, thread_grids& tgrids) noexcept{
		tgrids.heat_g[thisCell] = -5;
		return;
	}
};


class Reactor: public Component{
	public:
	static const bool acceptsHeat = false;
//	static constexpr Setup()
//	adjacency REACTOR(){
//		this.ticks++;
//	}
//	adjacency DEFAULT(){
//		adjComponents+= (adj.acceptsHeat)
//	}
//	adjEffect<T::acceptsHeat,variables::adjComponents,operator+>
	
	static const void component_setup(gsize_t thisCell, thread_grids& tgrids) noexcept{
		Component::component_setup(thisCell, tgrids);
		//locala_g[lin(x,y)] = sum_adjacent_with_property(thisCell,x,y,acceptsHeat)
		
	}
	static const void component_action(gsize_t thisCell, thread_grids& tgrids) noexcept{
		adjacency_t* this_adjacency = tgrids.adjacency_sg+(thisCell*NUM_COMPONENT_TYPES);
		auto numReactors = this_adjacency[REACTOR_ID]+1;
		tgrids.energy_g[thisCell] = numReactors;
		tgrids.heat_g[thisCell] = numReactors*numReactors;
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
result_t scoreCurrentGrid() noexcept{
		res_cell heatSum = sum_grid<>(heat1_g, GRID_SIZE);
		if(heatSum > 0){
			return -static_cast<result_t>(heatSum);
		}
		int thisSum = sum_grid(energy1_g, GRID_SIZE);
		return static_cast<result_t>(thisSum);
}
	

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
