//gridManip.h - contains grid manipulation methods
#pragma once
#include <cstring>
#include "types.hpp"
//functions for detecting if the given index is on an edge of the grid.
inline bool is_left_edge(gsize_t index) noexcept{
	return (index%GRID_X_SIZE == 0);
}
inline bool is_right_edge(gsize_t index) noexcept{
	return ((index+1)%GRID_X_SIZE == 0);
}
inline bool is_top_edge(gsize_t index) noexcept{
	return (index < GRID_X_SIZE);
}
inline bool is_bottom_edge(gsize_t index) noexcept{
	return (index >= ((GRID_Y_SIZE-1)*GRID_X_SIZE));
}

//returns linear offset given an x offset - X can be negative!
inline goffset_t cell_XOffset(goffset_t deltaX) noexcept{
	return deltaX;//*std::static_cast<goffset_t>(sizeof cell)
}
//returns linear offset given a y offset - y can be negative!
inline goffset_t cell_YOffset(goffset_t deltaY, gsize_t xSize) noexcept{
	return deltaY* (static_cast<goffset_t>(xSize));//*std::static_cast<goffset_t>(sizeof cell)
}

inline gsize_t cell_YPos(gsize_t cellIndex) noexcept{
	return cellIndex/GRID_X_SIZE;
}

inline gsize_t cell_XPos(gsize_t cellIndex) noexcept{
	return cellIndex%GRID_X_SIZE;
}

//turns (x,y) coords into a linear offset - x,y CANNOT be negative!
inline gsize_t cell_linear_offset(gsize_t x,gsize_t y) noexcept{
	return x+(y*GRID_X_SIZE);
}

//get the address of the neighboring cell.
inline cell* cell_neighbor(cell* thisCell, goffset_t deltaX, goffset_t deltaY) noexcept{
	return thisCell + cell_XOffset(deltaX) + cell_YOffset(deltaY, GRID_X_SIZE);
}

//index offset
inline gsize_t cell_neighbor_offset(gsize_t thisOffset, goffset_t deltaX, goffset_t deltaY) noexcept{
	return thisOffset+cell_XOffset(deltaX) + cell_YOffset(deltaY, GRID_X_SIZE);
}

//sums the resources of type RESOURCE for grid GRID
template<typename C>
C sum_grid(C* grid, gsize_t gridSize) noexcept{
	C total = 0;
	//cell* resGrid = grid+(gridSize * resource);
	for(gsize_t i = 0; i < gridSize; i++){
		total+= (grid[i]);
	}
	return total;
}

//do i want to do this for just 4? or somehow do like a radius thing?
template<typename Functor>
void doForAdjacents(function_args& tlocals){
	auto thisCell = tlocals.thisCell;
	gsize_t neighborIndex;
	if(!is_right_edge(thisCell)){
		neighborIndex = cell_neighbor_offset(thisCell,1,0);
		Functor::func(tlocals,neighborIndex);	
	}
	if(!is_left_edge(thisCell)){
		neighborIndex = cell_neighbor_offset(thisCell, -1,0);
		Functor::func(tlocals,neighborIndex);
	}
	if(!is_bottom_edge(thisCell)){
		neighborIndex = cell_neighbor_offset(thisCell, 0,1);
		Functor::func(tlocals,neighborIndex);
	}
	if(!is_top_edge(thisCell)){
		neighborIndex = cell_neighbor_offset(thisCell, 0,-1);
		Functor::func(tlocals,neighborIndex);
	}
	return;	
}


struct adjTypeSum{
	void func(function_args& tlocals, gsize_t neighborIndex){
		auto adjType = tlocals.type_g[neighborIndex];
		auto adjComponents = tlocals.adjacency_sg+(tlocals.thisCell*NUM_COMPONENT_TYPES);
		adjComponents[adjType]++;
		return;
	}
};


//may be able to do this in parallel
//gets how many of each component are adjacent to this component, stores it in adjacency_sg.
void sumAdjacentComponents(function_args& tlocals) noexcept
{
	auto thisCell = tlocals.thisCell;
	auto adjComponents = tlocals.adjacency_sg+(thisCell*NUM_COMPONENT_TYPES);
	auto tCell = tlocals.type_g+thisCell;
	cell* adjCell;
	for(int i = 0; i < 4; i++){
		bool invalid = false;
		switch(i){
			case 0:
			adjCell = cell_neighbor(tCell, 1,0);
			invalid = is_right_edge(thisCell);
			break;
			case 1:
			adjCell = cell_neighbor(tCell, -1,0);
			invalid = is_left_edge(thisCell);
			break;
			case 2:
			adjCell = cell_neighbor(tCell, 0,1);
			invalid = is_bottom_edge(thisCell);
			break;
			case 3:
			adjCell = cell_neighbor(tCell, 0,-1);
			invalid = is_top_edge(thisCell);
			break;
		}
		if(!invalid){
			//printf("%d:%d\n",thisCell,*adjCell);
			adjComponents[*adjCell]++;
		}
	}
	return;
}

//clears all grids except for 'type_g'
inline void clear_non_type_grids(function_args& tlocals) noexcept{
	memset(tlocals.adjacency_sg,0,sizeof(tlocals.adjacency_sg[0])*GRID_SIZE*NUM_COMPONENT_TYPES);
	memset(tlocals.energy_g,0,sizeof(tlocals.energy_g[0])*GRID_SIZE);
	memset(tlocals.heat_g,0,sizeof(tlocals.heat_g[0])*GRID_SIZE);
	memset(tlocals.locals_g,0,sizeof(tlocals.locals_g[0])*GRID_SIZE);
	tlocals.resNet.reset();
	tlocals.endpointList.clear();
	return;
}

//implemented in gridManip_after.hpp
int setup_grid(function_args& tlocals) noexcept;
int run_grid(function_args& tlocals) noexcept;
//implemented in components.hpp
result_t scoreCurrentGrid() noexcept;//score the current grid. should not be called before sim().

//sim the grid typegrid.
void sim(function_args& tlocals) noexcept{
	clear_non_type_grids(tlocals);
	setup_grid(tlocals);
	run_grid(tlocals);
	if(COUNT_SIMS){
		count_sims_low++;
	}
	return;
}

//increment the grid by 1. used in brute force search.
void increment_grid(function_args& tlocals) noexcept{
	tlocals.type_g[0]++;
	//if grid[0] == NUM_COMPONENT_TYPES, carry
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		if(tlocals.type_g[i] == NUM_COMPONENT_TYPES){
			tlocals.type_g[i] = 0;
			tlocals.type_g[i+1]++;
		}
		else{
			return;
		}
	}
}

