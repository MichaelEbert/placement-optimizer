#pragma once
#include <array>
#include "gridManip_after.hpp"

//std::uniform_int_distribution<int> uniform_dist(1, 6);
std::mt19937 rng;
std::uniform_int_distribution<std::mt19937::result_type> randComponent(0,NUM_COMPONENT_TYPES-1); // distribution in range [0,NUM_TYPES-1]
std::uniform_int_distribution<std::mt19937::result_type> randCellIndex(0,GRID_SIZE-1);
std::uniform_real_distribution<float> randFloat(0,1);
std::exponential_distribution<float> randExpFloat(2);
//use: randType(rng) to return a random component type.

namespace genetic{
	const unsigned int POP_SIZE = 500;//population size
	const unsigned int NUM_GENERATIONS = 1000;//how many generations should we simulate 
	const float MUTATION_CHANCE = 0.55f;
	const int TOP_PARENTS_RESERVED_SLOTS = 20;//the top n parents will be copied into the next generation
	const int numThreads = 6;
}
typedef struct{
	int index;
	result_t result;
} Ranking;

//reverse sort - biggest first
bool rankingSort(const Ranking& a, const Ranking& b){return a.result > b.result;}
int rankingAdd(const int& a, const Ranking& b){return a + std::max(b.result,0);}

