#pragma once
#include <cstdio>
//types.h - component types


//brute force search
//this won't work for real- 4 types 20 slots in 1 hour requires 300M tests/sec.
//no premature optimization! just do everything serial right now.
//but as a start, it'll work.
const short NUM_COMPONENT_TYPES = 4;
const short NUM_RESOURCE_TYPES = 2;
const short GRID_X_SIZE = 5;
const short GRID_Y_SIZE = 3;

const int GRID_SIZE = GRID_X_SIZE*GRID_Y_SIZE;
typedef unsigned char cell;
typedef cell* grid;
typedef unsigned int gsize_t;
typedef signed long goffset_t;//max offset = entire grid.

inline void sumAdjacentComponents(cell* thisCell, gsize_t x, gsize_t y, unsigned char* adjComponents) noexcept;
inline gsize_t cell_linear_offset(gsize_t x, gsize_t y) noexcept;
//switch between component setups with functor array, populated by fctr_arr[COMP_TYPE_ID] = COMP_TYPE::fctr;

//best is probably to only write to your cell if possible. more parallel that way.

static cell type_grid[GRID_SIZE];
static cell resource_grid[NUM_RESOURCE_TYPES][GRID_SIZE];

//setup function type: cell* thisCell,gsize_t thisX,gsize_t thisY -> void
typedef const void (*setup_func_t)(cell*,gsize_t,gsize_t);

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

class Component{
public:
	static const void component_setup(cell* thisCell, gsize_t x, gsize_t y) noexcept{
		return;
	}
};

class None : public Component{
	public:
		static const void component_setup(cell* thisCell, gsize_t x, gsize_t y) noexcept{
			return;
		}
};

class Spreader : public Component{
public:
	
};

class HeatSink: public Component{
public:
	static const signed int HEATSINK_HEAT_START = -5;
	static const void component_setup(cell* thisCell, gsize_t x, gsize_t y) noexcept{
		unsigned char adjComponents[NUM_COMPONENT_TYPES];
		sumAdjacentComponents(thisCell, x, y, adjComponents);
		resource_grid[RES_HEAT_ID][cell_linear_offset(x,y)] = -1;
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
	
	static const void component_setup(cell* thisCell, gsize_t x, gsize_t y) noexcept{
		unsigned char adjComponents[NUM_COMPONENT_TYPES];
		sumAdjacentComponents(thisCell, x, y, adjComponents);
		resource_grid[RES_ENERGY_ID][cell_linear_offset(x,y)] = adjComponents[REACTOR_ID] + 1;
		resource_grid[RES_HEAT_ID][cell_linear_offset(x,y)] = adjComponents[REACTOR_ID] + 1;
		//*thisCell=adjComponents[REACTOR_ID] + 1;//ticks
		//this->adjComponents=adjComponents[HEATSINK_ID] + adjComponents[SPREADER_ID];
	}
};

//setup array

setup_func_t component_setup_arr[] = {
	None::component_setup,
	Reactor::component_setup,
	HeatSink::component_setup,
	Spreader::component_setup	
};


template <typename T,size_t N>
constexpr size_t array_size(T(&)[N]){
	return N;
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
