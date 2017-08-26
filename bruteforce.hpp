#pragma once
#include <memory>
#include <vector>
#include "gridManip_after.hpp"
//brute force method

/*
multithreading strategy:
divide up the work by having each thread have the last N components in the array be fixed. (for some value of N, for even enough computation) 
wu = "work unit"
*/
namespace threads{
	const int num_static_slots = 2;//how many spots in the array are we going to specify
	const int num_threads = 1;
	const unsigned int iterations_per_wu = std::pow(NUM_COMPONENT_TYPES,GRID_SIZE-num_static_slots);
	const unsigned int total_work_units = std::pow(NUM_COMPONENT_TYPES,num_static_slots);
	constexpr float work_units_per_thread = static_cast<float>(total_work_units)/num_threads;
	
	static_assert(work_units_per_thread>4,"Highly variable work per thread. Recommend increasing NUM_STATIC_SLOTS.");
	
	//thread index to which wu to start at
	int index_to_starting_wu(int index){
		return work_units_per_thread * index;
	}
	//threads may have different amounts of work units if wu's don't evenly divide into threads.
	//calculate how many wu's to do given a starting wu.
	int wu_to_do(int index){
		return index_to_starting_wu(index+1)-index_to_starting_wu(index);
	}
	
	//transforms a thread index index into the proper starting wu index element
	int index_to_wu_value(int threadIndex, int arrayIndex){
		auto wu = index_to_starting_wu(threadIndex);
		return (wu/static_cast<int>(std::pow(NUM_COMPONENT_TYPES,arrayIndex)))%NUM_COMPONENT_TYPES;
	}
}


void bruteForceWork(int threadnum, cell* threadBestGrid, int* threadBestScore){
	cell bestGrid[GRID_SIZE];
	//thread-local grids
	cell type_g[GRID_SIZE];
	adjacency_t adjacency_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
	res_cell energy_g[GRID_SIZE];
	res_cell heat_g[GRID_SIZE];
	LocalVars locals_g[GRID_SIZE];
	ResourceNetworkManager<res_cell> resNet;
	
	function_args locals_test;
	locals_test.thisCell = 0;
	locals_test.typegrid = type_g;
	locals_test.adjacency_sg = adjacency_sg;
	locals_test.energy_g = energy_g;
	locals_test.heat_g = heat_g;
	locals_test.locals_g = locals_g;
	locals_test.resNet = resNet;
	
	int bestSoFar = 0;
	memset(type_g, 0, GRID_SIZE);
	//initialize thread stuff
	unsigned long num_iterations = threads::wu_to_do(threadnum)*threads::iterations_per_wu;
	for(int i = threads::num_static_slots; i >0; i--){
		type_g[GRID_SIZE-i] = threads::index_to_wu_value(threadnum,i-1);
	}
	for(unsigned long i = 0; i < num_iterations; i++){
		increment_grid(locals_test);
		sim(locals_test);
		int thisSum = scoreCurrentGrid(locals_test);
//		printMatrix(type_g);
//		printf("result is %d\n",thisSum);
		if(thisSum > bestSoFar){
			bestSoFar = thisSum;
			//bestHeat = heatSum;
			memcpy(bestGrid,type_g,GRID_SIZE*sizeof(cell));
		}
	}
	std::copy(std::begin(bestGrid),std::end(bestGrid),threadBestGrid);
	*threadBestScore = bestSoFar;
}

//solve using exhaustive search. multithreaded.

std::unique_ptr<cell[]> bruteForce(){
	std::unique_ptr<cell[]> bestGrid = std::make_unique<cell[]>(GRID_SIZE);
	
//	{//user-defined grid instead of brute forcing this
//		cell testGrid[GRID_SIZE] = {
//					REACTOR_ID,HEATSINK_ID,BOILER_ID,
//					HEATSINK_ID,BOILER_ID,NONE_ID,
//					BOILER_ID,NONE_ID,NONE_ID,
//					0,0,0};
//		std::copy(std::begin(testGrid),std::end(testGrid),bestGrid.get());
//	}
//	return bestGrid;
	//------------actual algorithm
	int bestSoFar = 0;
	int bestHeat = 0;
	cell* threadBestGrid[threads::num_threads];
	int threadBestScore[threads::num_threads];
	
	for(int i = 0; i < threads::num_threads; i++){
		threadBestGrid[i] = static_cast<cell*>(malloc(sizeof(cell)*GRID_SIZE));
		memset(threadBestGrid[i],0,sizeof(cell)*GRID_SIZE);
	}
	
	unsigned long num_iterations = (threads::wu_to_do(0)+1)*threads::iterations_per_wu;
	printf("iterations: %u (expected time %2f seconds +- 50%%)\n",num_iterations,num_iterations/4000000.0f);

	//split into threads and work
	std::vector<std::thread> workThreads;
	for(int i = 0; i < threads::num_threads; i++){
		workThreads.emplace_back(std::thread(bruteForceWork,i,threadBestGrid[i],&threadBestScore[i]));
	}
	for(auto& thread:workThreads){
		thread.join();
	}
	
	//compute best result from all threads
	auto maxElementIt = std::max_element(std::begin(threadBestScore),std::end(threadBestScore));
	auto maxElementIndex = std::distance(std::begin(threadBestScore),maxElementIt);
	std::copy(threadBestGrid[maxElementIndex],threadBestGrid[maxElementIndex]+(sizeof(cell)*GRID_SIZE),bestGrid.get());
	
	for(int i = 0; i < threads::num_threads; i++){
		free(threadBestGrid[i]);
	}
	return bestGrid;
}
