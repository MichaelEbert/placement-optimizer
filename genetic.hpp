#pragma once
#include <random>
#include <algorithm>
#include <numeric>
#include "genetic_types.hpp"
#include "simTiming.hpp"

void printResultsToFile(int topa, int topb, int topc, char* filename) {
	auto file = fopen(filename, "ab");
	//format: top 3 scores, popsize, generations, mutation chance, top parent reserved slots
	fprintf(file, "%d,%d,%d,%d,%d,%f,%d\n", topa, topb, topc, genetic::POP_SIZE, genetic::NUM_GENERATIONS, genetic::MUTATION_CHANCE * 100, genetic::TOP_PARENTS_RESERVED_SLOTS);
	fclose(file);
}


/*
algorithm:
seed:
	generate POP_SIZE arrangements
loop:
	evaluate():
		run each arrangement, record results.
	reproduce():
		create next generation from top arrangements. 
*/


//allocate on the heap because its larger
//TEST: performance of single block vs multiple blocks. cache might have somehting to say about this.
cell* candidates;
cell* newCandidates;

std::array<Ranking, genetic::POP_SIZE> popRankings;

static inline cell* candidateIx(int index){
	return candidates+(index*GRID_SIZE);
}
static inline cell* newCandidateIx(int index){
	return newCandidates+(index*GRID_SIZE);
}

//allocates memory, initializes rng(), and (randomly) seeds the initial generation.
void seed(function_args& tlocals){
    rng.seed(std::random_device()());
    candidates = static_cast<cell*>(malloc(sizeof(cell)*GRID_SIZE*genetic::POP_SIZE));
    newCandidates = static_cast<cell*>(malloc(sizeof(cell)*GRID_SIZE*genetic::POP_SIZE));
	if (!candidates || !newCandidates) {
		printf("Out of memory. Terminating.\n");
		exit(ENOMEM);
	}
    for(unsigned int i = 0; i < genetic::POP_SIZE; i++){
    	cell* curCandidate = candidates+(i*GRID_SIZE);
    	for(gsize_t c = 0; c < GRID_SIZE; c++){
    		curCandidate[c] = randComponent(rng);
    	}
		tlocals.type_g = curCandidate;
		sim(tlocals);
		if (scoreCurrentGrid(tlocals) < 0) {
			i--;
		}
    }
}

//evaluate every candidate in a generation.
void evaluate(function_args& tlocals){
	for(unsigned int i = 0; i < genetic::POP_SIZE; i++){
		tlocals.type_g = candidates+(i*GRID_SIZE);
		sim(tlocals);
		popRankings[i].index = i;
		popRankings[i].result = scoreCurrentGrid(tlocals);
	}
	std::sort(popRankings.begin(),popRankings.end(),rankingSort);
}


//probability of i being selected as a parent = (i.score)/(total score); <- that doesnt work. doesn't give enough weight to better ones.
//trying exponential generation of the initial number.
//select parent -  place for heuristics!
int get_parent(int totalWeight){
	
	float randomFloat = randExpFloat(rng);//hopefully between 0 and 1
	while(randomFloat >= 1.0f){
		randomFloat = randExpFloat(rng);
	}
	float curWeight = 0;
	unsigned int i = 0;
	curWeight = popRankings[i].result/static_cast<float>(totalWeight);
	while(randomFloat >= curWeight && randomFloat > (1/static_cast<float>(totalWeight))){
		randomFloat -= curWeight;
		i++;
		curWeight = popRankings[i].result/static_cast<float>(totalWeight);
	}
	if(i>= genetic::POP_SIZE){
		return 0;
	}
	return i;
}

//going to try the "pick a random point, everything before is from A, everything after is from B".
void mate(cell* parentA, cell* parentB, cell* child){
	int midIndex = randCellIndex(rng);
	memcpy(child,parentA,midIndex*sizeof(cell));
	memcpy(child+midIndex,parentB+midIndex,sizeof(cell)*(GRID_SIZE - midIndex));
	return;
}

//every reproduction has a chance of a mutation
void mutate(int child){
	if(randFloat(rng) > genetic::MUTATION_CHANCE){
		int thiscell = randCellIndex(rng);
		cell component = static_cast<cell>(randComponent(rng));
		//printf("%d,%d\n",thiscell,component);
		if(thiscell > GRID_SIZE || component < 0 || component >= NUM_COMPONENT_TYPES){
			newCandidateIx(child)[thiscell] = component;
			return;
		}
		newCandidateIx(child)[thiscell] = component;
	}	
}

//create new generation from top arrangements
//run the sim immediately, if the child is not viable (v < 0), try a new child.
void reproduce(function_args& tlocals){
	int totalWeight = std::accumulate(popRankings.begin(),popRankings.end(),0,rankingAdd);
	for(unsigned int i = 0; i < genetic::POP_SIZE- genetic::TOP_PARENTS_RESERVED_SLOTS; i++){
		int parent1 = get_parent(totalWeight);
		int parent2 = get_parent(totalWeight);
		mate(candidateIx(parent1),candidateIx(parent2),newCandidateIx(i));
		mutate(i);
		
		//run the sim immediately, if the child is not viable (v < 0), try a new child.
		tlocals.type_g = newCandidateIx(i);
		sim(tlocals);
		if(scoreCurrentGrid(tlocals) < 0){
			i--;
		}	
	}
	int parentOffset = genetic::POP_SIZE-genetic::TOP_PARENTS_RESERVED_SLOTS;
	for(unsigned int i = parentOffset; i < genetic::POP_SIZE; i++){
		memcpy(newCandidateIx(i),candidateIx(popRankings[i-parentOffset].index),sizeof(cell)*GRID_SIZE);
	}
	std::swap(candidates,newCandidates);
}

/*
multithreading strategy:
divide up evaluate() into multiple threads?

*/

//called by main().
std::unique_ptr<cell[]> geneticSolve() {
	//look up pruning genetic algorithm - heuristic
	std::unique_ptr<cell[]> bestGrid = std::make_unique<cell[]>(GRID_SIZE);

	double timePerSim = getTimingInfo();
	double numSims = genetic::POP_SIZE*genetic::NUM_GENERATIONS;
	double simSeconds = timePerSim*numSims / 1000;
	printf("genetic simulation expected to take well over %2.2f seconds", simSeconds);
	if (simSeconds > 300) {
		printf(" (%2.0 minutes)", simSeconds / 60);
	}
	printf("\n");

	cell type_g[GRID_SIZE];
	adjacency_t adjacency_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
	res_cell energy_g[GRID_SIZE];
	res_cell heat_g[GRID_SIZE];
	LocalVars locals_g[GRID_SIZE];
	ResourceNetworkManager<res_cell> resNet;

	function_args threadlocals;
	threadlocals.thisCell = 0;
	threadlocals.type_g = type_g;
	threadlocals.adjacency_sg = adjacency_sg;
	threadlocals.energy_g = energy_g;
	threadlocals.heat_g = heat_g;
	threadlocals.locals_g = locals_g;
	threadlocals.resNet = resNet;

	seed(threadlocals);
	if (count_sims_low > ONE_BILLION) {
		count_sims_low -= ONE_BILLION;
		count_sims_high++;
	}
	printf("initial round seeded in %u iterations\n", count_sims_low);
	for (int i = 0; i < genetic::NUM_GENERATIONS; i++) {
		evaluate(threadlocals);
		reproduce(threadlocals);
		//count simulations
		if (count_sims_low > ONE_BILLION) {
			count_sims_low -= ONE_BILLION;
			count_sims_high++;
		}
		auto tenPercent = genetic::NUM_GENERATIONS / 10;
		if (i % tenPercent == tenPercent - 1) {
			printf("%d%% complete\n", (i+1) * 10 / tenPercent);
		}
		
	}
	printf("total sims: %d,%03d,%03d,%03d\n",
		count_sims_high,
		(count_sims_low / 1000000) % 1000,
		(count_sims_low / 1000) % 1000,
		count_sims_low % 1000);


	cell tempGrid[GRID_SIZE];
	int bestSoFar;
	printf("top 3 are\n");
	for (int i = 0; i < 3; i++) {
		bestSoFar = popRankings[i].result;
		memcpy(tempGrid, candidates + (popRankings[i].index*GRID_SIZE), GRID_SIZE * sizeof(cell));
		if (i == 0) {
			std::copy(std::begin(tempGrid), std::end(tempGrid), bestGrid.get());
		}
		printf("\nbest value is %d\n", bestSoFar);
		//printf("with heat value %d\n",bestHeat);
		printMatrix(tempGrid);
	}
	//	printResultsToFile(popRankings[0].result,popRankings[1].result,popRankings[2].result,"geneticResults.csv");
	return bestGrid;
}


