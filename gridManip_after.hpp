#pragma once
#include "componentMethods.hpp"

//do the setup functions for each component
int setup_grid(function_args& tlocals) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		tlocals.thisCell = i;
		component_setup_arr[tlocals.typegrid[i]](tlocals);
	}
	return 0;
}

//do the run function for each component
int run_grid(function_args& tlocals) noexcept{
	for(gsize_t i = 0; i < GRID_SIZE; i++){
		tlocals.thisCell = i;
		component_action_arr[tlocals.typegrid[i]](tlocals);
	}
	return 0;
}
