#pragma once
#include <memory>
#include <vector>
#include <chrono>
#include <algorithm>
#include "gridManip_after.hpp"
#include "types.hpp"
//brute force method

/*
multithreading strategy:
divide up the work by having each thread have the last N components in the array be fixed. (for some value of N, for even enough computation) 
wu = "work unit"
*/
namespace threads{
	const unsigned int num_static_slots = 2;//how many spots in the array are we going to specify
	const unsigned int num_threads = 6;
	const unsigned int iterations_per_wu = const_pow<unsigned int>(NUM_COMPONENT_TYPES,GRID_SIZE-num_static_slots);
	const unsigned int total_work_units = const_pow<unsigned int>(NUM_COMPONENT_TYPES,num_static_slots);
	constexpr float work_units_per_thread = static_cast<float>(total_work_units)/num_threads;
	
	static_assert(work_units_per_thread>4,"Highly variable work per thread. Recommend increasing NUM_STATIC_SLOTS.");
	
	
	//thread index to which wu to start at
	int index_to_starting_wu(int index){
		return static_cast<int>(work_units_per_thread * index);
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

//returns expected time to do 1 wu.
double getTimingInfo(){
	printf("running timing tests...\n");
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
	
	memset(type_g, 0, GRID_SIZE);
	
	//get into a middle state
	for(int i = 0; i < std::max(1,NUM_COMPONENT_TYPES*GRID_SIZE-4); i++){
		increment_grid(locals_test);
	}
	int bestSoFar = 0;
	
	//time 100 iterations
	
	int iterations = 16;
	unsigned int diffms = 0;
	//keep simming until we get a 500ms run
	while(diffms < 500){
		iterations = iterations*4;
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++){
			increment_grid(locals_test);
			sim(locals_test);
			auto thisSum = scoreCurrentGrid(locals_test);
			if(thisSum > bestSoFar){
				bestSoFar = thisSum;
			}
		}
		auto end = std::chrono::steady_clock::now();
		diffms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	}
	printf("%i\n",bestSoFar);
	printf("timing test finished (%u ms).\n",diffms);
	//printf("(%i simulations in %u ms)\n",iterations, diffms);
	
	double timePerIteration = static_cast<double>(diffms)/iterations;//in ms
	double timePerWu = timePerIteration * threads::iterations_per_wu;//in ms
	double expectedTime = timePerWu*threads::total_work_units/threads::num_threads/1000;//in seconds
	printf("expected time to complete: %2.0f seconds",expectedTime);
	if(expectedTime > 300){
		printf(" (%2.1f minutes)",expectedTime/60);
	}
	printf("\n\n");
	return timePerWu;
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
	printf("thread %d has %u wu.\n",threadnum,threads::wu_to_do(threadnum));
	for(int i = threads::num_static_slots; i >0; i--){
		type_g[GRID_SIZE-i] = threads::index_to_wu_value(threadnum,i-1);
	}
	
	unsigned long num_iterations = threads::wu_to_do(threadnum)*threads::iterations_per_wu;
	for(unsigned int wu = 0; wu < threads::wu_to_do(threadnum);wu++){
		for(unsigned long i = 0; i < threads::iterations_per_wu; i++){
			increment_grid(locals_test);
			sim(locals_test);
			int thisSum = scoreCurrentGrid(locals_test);
			if(thisSum > bestSoFar){
				bestSoFar = thisSum;
				//bestHeat = heatSum;
				memcpy(bestGrid,type_g,GRID_SIZE*sizeof(cell));
			}
	//		if(i %10000 == 0){
	//			printf("thread %d, bestresult %d.\n",threadnum,bestSoFar);
	//		}
	//		if(thisSum == 940){
	//			printf("i is %d\n",i);
	//				printf("optimal result:\n");
	//				printMatrix(bestGrid);
	//				printf("score: %d\n",thisSum);
	//				printf("energy grid:\n");
	//				printMatrix(energy_g);
	//				printf("heat grid:\n");
	//				printMatrix(heat_g);
	//				printf("locals grid:\n");
	//				printMatrix(locals_g);
	//				printf("network info:\n");
	//				locals_test.resNet.printDebugInfo();
	//			break;
	//		}
		}
		printf("thread %d completed work unit %d; best result %d.\n",threadnum,wu,bestSoFar);
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
////		cell testGrid[GRID_SIZE] = {
////					REACTOR_ID,SPREADER_ID,REACTOR_ID,
////					SPREADER_ID,HEATSINK_ID,REACTOR_ID,
////					REACTOR_ID,REACTOR_ID,NONE_ID};
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
		if (threadBestGrid[i] == nullptr) {
			printf("Out of memory. Terminating.\n");
			exit(ENOMEM);
		}
		memset(threadBestGrid[i],0,sizeof(cell)*GRID_SIZE);
	}
	
	getTimingInfo();

	//printf("%u iterations per work unit.\n",threads::iterations_per_wu);
	//split into threads and work
	if(threads::num_threads>1){
		std::vector<std::thread> workThreads;
		for(int i = 0; i < threads::num_threads; i++){
			workThreads.emplace_back(std::thread(bruteForceWork,i,threadBestGrid[i],&threadBestScore[i]));
		}
		for(auto& thread:workThreads){
			thread.join();
		}
	}
	else{
		bruteForceWork(0,threadBestGrid[0],&threadBestScore[0]);
	}
	
	//compute best result from all threads
	auto maxElementIt = std::max_element(std::begin(threadBestScore),std::end(threadBestScore));
	auto maxElementIndex = std::distance(std::begin(threadBestScore),maxElementIt);
	std::copy(threadBestGrid[maxElementIndex],threadBestGrid[maxElementIndex]+(sizeof(cell)*GRID_SIZE),bestGrid.get());
	
	printf("best score: %d\n",*maxElementIt);
	
	for(int i = 0; i < threads::num_threads; i++){
		free(threadBestGrid[i]);
	}
	return bestGrid;
}
