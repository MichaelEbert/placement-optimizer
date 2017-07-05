#pragma once
#include "components.hpp"



//TODO: ensure valid setups (is even possible?)
int setup_grid(function_args& tlocals) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		tlocals.thisCell = i;
		component_setup_arr[tlocals.typegrid[i]](tlocals);
	}
	return 0;
}

//TODO: ensure valid runs (is even possible?)
int run_grid(function_args& tlocals) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		tlocals.thisCell = i;
		component_action_arr[tlocals.typegrid[i]](tlocals);
	}
	return 0;
}

//increment the grid by 1. used in brute force search.
void increment_grid(function_args& tlocals) noexcept{
	tlocals.typegrid[0]++;
	//if grid[0] == NUM_COMPONENT_TYPES, carry
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		if(tlocals.typegrid[i] == NUM_COMPONENT_TYPES){
			tlocals.typegrid[i] = 0;
			tlocals.typegrid[i+1]++;
		}
		else{
			return;
		}
	}
}

//may be able to do this in parallel
//gets how many of each component are adjacent to this component, stores it in adjacency_sg.
void sumAdjacentComponents(function_args& tlocals) noexcept
{
	auto thisCell = tlocals.thisCell;
	auto adjComponents = tlocals.adjacency_sg+(thisCell*NUM_COMPONENT_TYPES);
	auto tCell = tlocals.typegrid+thisCell;
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
	memset(tlocals.adjacency_sg,0,sizeof(tlocals.adjacency_sg));
	memset(tlocals.energy_g,0,sizeof(tlocals.energy_g));
	memset(tlocals.heat_g,0,sizeof(tlocals.heat_g));
	memset(tlocals.properties_g,0,sizeof(tlocals.properties_g));
	return;
}

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


