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
