#pragma once
#include <algorithm>
#include <chrono>
#include "types.hpp"
#include "gridManip_after.hpp"

//returns expected time to do 1 simulation (in milliseconds).
double getTimingInfo() {
	printf("running timing tests...\n");
	//thread-local grids
	cell type_g[GRID_SIZE];
	adjacency_t adjacency_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
	res_cell energy_g[GRID_SIZE];
	res_cell heat_g[GRID_SIZE];
	LocalVars locals_g[GRID_SIZE];
	ResourceNetworkManager<res_cell> resNet;

	function_args threadlocals;
	threadlocals.thisCell = 0;
	threadlocals.typegrid = type_g;
	threadlocals.adjacency_sg = adjacency_sg;
	threadlocals.energy_g = energy_g;
	threadlocals.heat_g = heat_g;
	threadlocals.locals_g = locals_g;
	threadlocals.resNet = resNet;

	memset(type_g, 0, GRID_SIZE);

	//get into a middle state
	for (int i = 0; i < std::max(1, NUM_COMPONENT_TYPES*GRID_SIZE - 4); i++) {
		increment_grid(threadlocals);
	}

	int bestSoFar = 0;
	int iterations = 16;
	unsigned int diffms = 0;
	//keep simming until we get a 500ms run
	while (diffms < 500) {
		iterations = iterations * 4;
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			increment_grid(threadlocals);
			sim(threadlocals);
			auto thisSum = scoreCurrentGrid(threadlocals);
			if (thisSum > bestSoFar) {
				bestSoFar = thisSum;
			}
		}
		auto end = std::chrono::steady_clock::now();
		diffms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}
	printf("%i\n", bestSoFar);
	printf("timing test finished (%u ms).\n", diffms);
	//printf("(%i simulations in %u ms)\n",iterations, diffms);

	double timePerIteration = static_cast<double>(diffms) / iterations;//in ms
	return timePerIteration;
}