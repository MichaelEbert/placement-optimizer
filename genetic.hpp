#pragma once
#include <random>
#include <algorithm>

#include "genetic_types.hpp"



/*
algorithm:
seed:
	generate POP_SIZE arrangements
evaluate():
	run each arrangement, record results.
reproduce():
	create next generation from top arrangements. 
*/


//allocate on the heap because its larger
//TEST: performance of single block vs multiple blocks. cache might have somehting to say about this.
cell* candidates;
cell* newCandidates;

static inline cell* candidateIx(int index){
	return candidates+(index*GRID_SIZE);
}
static inline cell* newCandidateIx(int index){
	return newCandidates+(index*GRID_SIZE);
}

void seed(){
    rng.seed(std::random_device()());
    candidates = static_cast<cell*>(malloc(sizeof(cell)*GRID_SIZE*POP_SIZE));
    newCandidates = static_cast<cell*>(malloc(sizeof(cell)*GRID_SIZE*POP_SIZE));
    for(unsigned int i = 0; i < POP_SIZE; i++){
    	grid curCandidate = candidates+(i*GRID_SIZE);
    	for(gsize_t c = 0; c < GRID_SIZE; c++){
    		curCandidate[c] = randComponent(rng);
    	}
    }
}


void evaluate(){
	for(unsigned int i = 0; i < POP_SIZE; i++){
		grid curCandidate = candidates+(i*GRID_SIZE);
		
		function_args locals_test;
		locals_test.thisCell = 0;
		locals_test.typegrid = curCandidate;
		locals_test.adjacency_sg = adjacency1_sg;
		locals_test.energy_g = energy1_g;
		locals_test.heat_g = heat1_g;
		locals_test.properties_g = properties1_g;
		
		sim(locals_test);
		popRankings[i].index = i;
		popRankings[i].result = scoreCurrentGrid();
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
	while(randomFloat >= curWeight){
		randomFloat -= curWeight;
		i++;
		curWeight = popRankings[i].result/static_cast<float>(totalWeight);
	}
	if(i>=POP_SIZE){
		return 0;
	}
	return i;
}

//going to try the "pick a random point, everything before is from A, everything after is from B".
void mate(grid parentA, grid parentB, grid child){
	int midIndex = randCellIndex(rng);
	memcpy(child,parentA,midIndex*sizeof(cell));
	memcpy(child+midIndex,parentB+midIndex,sizeof(cell)*(GRID_SIZE - midIndex));
	return;
}

//every reproduction has a chance of a mutation
void mutate(int child){
	if(randFloat(rng) > MUTATION_CHANCE){
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
void reproduce(){
	int totalWeight = std::accumulate(popRankings.begin(),popRankings.end(),0,rankingAdd);
	for(unsigned int i = 0; i < POP_SIZE-TOP_PARENTS_RESERVED_SLOTS; i++){
		int parent1 = get_parent(totalWeight);
		int parent2 = get_parent(totalWeight);
		mate(candidateIx(parent1),candidateIx(parent2),newCandidateIx(i));
		mutate(i);
		
		//run the sim immediately, if the child is not viable (v < 0), try a new child.
		//sim(newCandidateIx(i));
		if(scoreCurrentGrid() < 0){
			i--;
		}	
	}
	int parentOffset = POP_SIZE-TOP_PARENTS_RESERVED_SLOTS;
	for(unsigned int i = parentOffset; i < POP_SIZE; i++){
		memcpy(newCandidateIx(i),candidateIx(popRankings[i-parentOffset].index),sizeof(cell)*GRID_SIZE);
	}
	std::swap(candidates,newCandidates);
}


	


