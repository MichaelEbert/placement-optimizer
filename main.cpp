//reactor placement brute-force
//input text file with 1s where you can place, 0s where you can't
#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <float.h>
#include <stdint.h>

#include "components.hpp"
#include "gridManip.hpp"

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

constexpr unsigned long brute_force_iterations = std::pow(NUM_COMPONENT_TYPES, GRID_SIZE);

//fix resGrid to be double pointer or someithng.
void sim(grid typeGrid, cell resGrid[][GRID_SIZE]) noexcept{
	memset(resGrid, 0, GRID_SIZE*NUM_RESOURCE_TYPES);
	setup_grid(typeGrid, GRID_SIZE);
	return;
}

int main(int argc, char** argv)
{
	cell bestGrid[GRID_SIZE];
	int bestSoFar = 0;
	int bestHeat = 0;
	memset(type_grid, 0, GRID_SIZE);
	printf("iterations: %u (expected time %2f seconds)\n",brute_force_iterations,brute_force_iterations/2000000.0f);
	for(unsigned long i = 0; i < brute_force_iterations; i++){
		increment_grid(type_grid, GRID_SIZE);
		sim(type_grid,resource_grid);
		int thisSum = sum_grid(resource_grid[RES_ENERGY_ID], GRID_SIZE, RES_ENERGY_ID);
		int heatSum = sum_grid(resource_grid[RES_HEAT_ID], GRID_SIZE, RES_HEAT_ID);
//		printf("\n\nsum is %d, heat is %d\n",thisSum,heatSum);
//		printMatrix(type_grid);
//printf("iteration %u\n",i);
		if(thisSum > bestSoFar && heatSum <= 0){
			bestSoFar = thisSum;
			bestHeat = heatSum;
			memcpy(bestGrid,type_grid,GRID_SIZE*sizeof(cell));
		}
		
		
	}
	printf("\nbest value is %d\n",bestSoFar);
	printf("with heat value %d\n",bestHeat);
	printMatrix(bestGrid);
	
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

