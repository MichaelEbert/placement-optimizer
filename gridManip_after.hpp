#pragma once
#include "components.hpp"



//TODO: ensure valid setups (is even possible?)
int setup_grid(grid typegrid) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		component_setup_arr[typegrid[i]](i, typegrid);
	}
	return 0;
}

//TODO: ensure valid runs (is even possible?)
int run_grid(grid typegrid) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		component_action_arr[typegrid[i]](i, typegrid);
	}
	return 0;
}

//increment the grid by 1. used in brute force search.
void increment_grid(grid typegrid) noexcept{
	typegrid[0]++;
	//if grid[0] == NUM_COMPONENT_TYPES, carry
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		if(typegrid[i] == NUM_COMPONENT_TYPES){
			typegrid[i] = 0;
			typegrid[i+1]++;
		}
		else{
			return;
		}
	}
}

//may be able to do this in parallel
//gets how many of each component are adjacent to this component, stores it in adjacency_sg.
void sumAdjacentComponents(gsize_t thisCell, grid typegrid) noexcept
{
	auto adjComponents = adjacency1_sg+(thisCell*NUM_COMPONENT_TYPES);
	auto tCell = typegrid+thisCell;
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
inline void clear_non_type_grids() noexcept{
	memset(adjacency1_sg,0,sizeof(adjacency1_sg));//sizeof doesn't work with pointers!
	memset(energy1_g,0,sizeof(energy1_g));
	memset(heat1_g,0,sizeof(heat1_g));
	memset(properties1_g,0,sizeof(properties1_g));
	return;
}

//sim the grid typegrid.
void sim(grid typegrid) noexcept{
	clear_non_type_grids();
	setup_grid(typegrid);
	run_grid(typegrid);
	if(COUNT_SIMS){
		count_sims_low++;
	}
	return;
}


