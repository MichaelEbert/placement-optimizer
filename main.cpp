//reactor placement brute-force
//input text file with 1s where you can place, 0s where you can't

#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <float.h>
#include <stdint.h>

/*
include order:
types.hpp->gridManip.hpp->components.hpp->gridManip_after.hpp->genetic_types.hpp->genetic.hpp->main.cpp

*/

//#include "gridManip_after.hpp"
#include "genetic.hpp"


//treat all non-integers as whitespace
struct integer_only : std::ctype<char>
{
	integer_only() : std::ctype<char>(get_table()) {}

	static std::ctype_base::mask const* get_table()
	{
		static std::vector<std::ctype_base::mask>
			rc(std::ctype<char>::table_size, std::ctype_base::space);

		std::fill(&rc['0'], &rc['9' + 1], std::ctype_base::digit);
		return &rc[0];
	}
};


int width = 0;
int height = 0;


int printMatrix(cell* matrix) noexcept;
int calcEfficiency(std::vector<std::vector<int>> matrix);


//constexpr unsigned long brute_force_iterations = std::pow(NUM_COMPONENT_TYPES, GRID_SIZE);
static cell type_g[GRID_SIZE];


void printResultsToFile(int topa,int topb,int topc,char* filename){
	auto file = fopen(filename,"ab");
	//format: top 3 scores, popsize, generations, mutation chance, top parent reserved slots
	fprintf(file, "%d,%d,%d,%d,%d,%f,%d\n",topa,topb,topc,POP_SIZE,NUM_GENERATIONS,MUTATION_CHANCE*100,TOP_PARENTS_RESERVED_SLOTS);
	fclose(file);
}

int main(int argc, char** argv)
{
	cell bestGrid[GRID_SIZE];
	int bestSoFar = 0;
	int bestHeat = 0;
	memset(type_g, 0, GRID_SIZE);
	printf("doing %u generations of %u size for a total of AT LEAST %u iterations, expecting to take over %2f seconds\n",NUM_GENERATIONS,POP_SIZE,POP_SIZE*NUM_GENERATIONS,POP_SIZE*NUM_GENERATIONS/4000000.0f);
//	printf("iterations: %u (expected time %2f seconds +- 50%%)\n",brute_force_iterations,brute_force_iterations/4000000.0f);
//	for(unsigned long i = 0; i < brute_force_iterations; i++){
//		increment_grid(type_g, GRID_SIZE);
//		sim(type_g);
//		int thisSum = scoreCurrentGrid();
//
////		printf("sum is %d, heat is %d\n",thisSum,heatSum);
////		printMatrix(type_g);
////printf("iteration %u\n\n",i);
//		if(thisSum > bestSoFar){
//			bestSoFar = thisSum;
//			//bestHeat = heatSum;
//			memcpy(bestGrid,type_g,GRID_SIZE*sizeof(cell));
//		}
//		
//		
//	}
//	printf("optimal result is %d\n",bestSoFar);
//	printMatrix(bestGrid);
//-----genetic algorithm-----
//look up pruning genetic algorithm - heuristic

	seed();
	for(int i = 0; i < NUM_GENERATIONS; i++){
		evaluate();
		reproduce();
		//count simulations
		if(count_sims_low > ONE_BILLION){
			count_sims_low-=ONE_BILLION;
			count_sims_high++;
		}
	}
	printf("total sims: %d,%03d,%03d,%03d\n",
	count_sims_high,
	(count_sims_low/1000000)%1000,
	(count_sims_low/1000)%1000,
	count_sims_low%1000);
	
	printf("top 3 are\n");
	for(int i = 0; i < 3; i++){
		bestSoFar = popRankings[i].result;
		memcpy(bestGrid,candidates+(popRankings[i].index*GRID_SIZE),GRID_SIZE*sizeof(cell));
		printf("\nbest value is %d\n",bestSoFar);
		//printf("with heat value %d\n",bestHeat);
		printMatrix(bestGrid);
	}
	printResultsToFile(popRankings[0].result,popRankings[1].result,popRankings[2].result,"geneticResults.csv");
	
//	cell tmp_arr[] = {REACTOR_ID,REACTOR_ID,REACTOR_ID,
//					REACTOR_ID,REACTOR_ID,REACTOR_ID,
//					NONE_ID,NONE_ID,NONE_ID};
//	memset(resource_grid, 0, 9);
//	setup_grid(tmp_arr, 9);
//	printMatrix(tmp_arr);
//	printMatrix(resource_grid);
//	int tmpSum = sum_grid(resource_grid, GRID_SIZE, 0);
//	printf("tmpSum is %d for size %d\n",tmpSum,GRID_SIZE);
	return 0;
}

/*
array positions:
[ ][ ][ ][ ][ ][ ][ ]
[ ][0][1][2][3][4][ ]
[ ][5][6][7][8][9][ ]
[ ][ ][ ][ ][ ][ ][ ]
*/

//todo: template to size of matrix?
int printMatrix(cell* matrix) noexcept
{
	char toPrint = 'x';
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			switch(matrix[cell_linear_offset(j,i)]){
//				case 0:
//					toPrint = '_';
//					break;
//				case 1:
//					toPrint = 'R';
//					break;
//				case 2:
//					toPrint = 'S';
//					break;
//				case 3:
//					toPrint = '+';
//					break;
				default:
					toPrint = '0'+matrix[cell_linear_offset(j,i)];
					break;
			}
			printf(" %c ", toPrint);
		}
		printf("\n");
	}
	return 0;
}

//void applyEffect(Effect& eff, Map& matrix, int curX, int curY){
//	return;
//}
//
//void applyEffects(int& j, std::vector<std::vector<int>>& compMatrix){
//	return;
//}
//
//int calcEfficiency(std::vector<std::vector<int>>& compMatrix){
//	for(auto i = compMatrix.begin();i != compMatrix.end();i++){
//		for(auto j = i.begin();j != i.end();j++){
//			applyEffects(j,compMatrix);
//		}
//	}
//}
//
////options to calculate effects:
////1. each cell calculates itself
////doesn't work for distributed things like heat - needs to be split evenly across adjacent cells, so hybrid model needed
//power+=1;
//forEach adjacentcell: if adjCell == REACTOR then power+=1;
//
//
////2. cell modifies adjacent cell properties
////need second pass after adjustments to total up
////-can we do every cell in sequence, or does there have to be a specific order?
////-how do we deal with negative power/heat? exchange capacity? -yep that should work.
//
////should second pass do any advanced functions?
////yes: reactor measured in ticks, in power sweep, ticks*
//power += 1;
//foreach adjCell in radius: if adjCell == REACTOR then adjCell.power++; adjCell.heatTicks++;
//		countNumAdjacentCellsOfType(REACTOR);
//		if adjCell != REACTOR then adjCell.heat+= (this.heatGen/this.adjHeatAcceptor)
//
//
//
////if we use bitflags, we can AND instead of IF'ing
////reminder: 0=>0,x=>1 is (x!=0)
//
////3. all cells implement 'auras', then evaluate aura effect.
////very functional - and slower.
//power+=1;
//temp adjHeatAcceptors = getAdjHeatAcceptors();
//foreach adjCell: adjCell.effects.add(REACTOR_HEAT(adjHeatAcceptors));
//...
//aura_REACTOR_HEAT:
//	
//	
////heat distribution: does timing matter?
////ex: 60 heat reactor, -10 comp, -50 comp:
////	react-even-sink = avg=50,10max = 70
////	react-sink-even = avg=50,10max = 50
////does the order of reactor, heat spreader, heat sink matter?
////NO* (2x entire reactor heat is good buffer, I think. less than that, need to do more math.)
////(see spreadsheet for details)
////***it seems that actual verification is a very hard problem that would require lots of processing..
////I should just use some parallelized filtering rules, and the ones that pass that get more extensively verified.
//
//
//
//

