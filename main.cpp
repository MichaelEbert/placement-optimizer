//reactor placement brute-force
//I apologize for my mix of camelCase and under_scores. christ this is horrific.
#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <float.h>
#include <stdint.h>
#include <thread>

/*
include order:
types.hpp->components.hpp->doForTypes.hpp->gridManip.hpp->printMatrix.hpp->componentMethods.hpp->gridManip_after.hpp->(genetic_types.hpp->genetic.hpp)OR(bruteforce.hpp)->main.cpp

*/

//#include "gridManip_after.hpp"
#include "types.hpp"
#include "genetic.hpp"
#include "bruteforce.hpp"
	
//we only save the optimal result positions, so we need to regenerate all other information for the optimal result
void displayOptimalResult(cell* bestGrid){
	adjacency_t adjacency_sg[GRID_SIZE*NUM_COMPONENT_TYPES];//this is a special one. so _gs instead of _g.
	res_cell energy_g[GRID_SIZE];
	res_cell heat_g[GRID_SIZE];
	LocalVars locals_g[GRID_SIZE];
	ResourceNetworkManager<res_cell> resNet;
	
	function_args locals_test;
	locals_test.thisCell = 0;
	locals_test.typegrid = bestGrid;
	locals_test.adjacency_sg = adjacency_sg;
	locals_test.energy_g = energy_g;
	locals_test.heat_g = heat_g;
	locals_test.locals_g = locals_g;
	locals_test.resNet = resNet;
	
	sim(locals_test);
	printf("optimal result:\n");
	printMatrix(bestGrid);
	printf("score: %d\n",scoreCurrentGrid(locals_test));
	printf("energy grid:\n");
	printMatrix(energy_g);
	printf("heat grid:\n");
	printMatrix(heat_g);
	printf("locals grid:\n");
	printMatrix(locals_g);
	printf("network info:\n");
	locals_test.resNet.printDebugInfo();
	return;
}
	

int main(int argc, char** argv)
{
	std::unique_ptr<cell[]> bestGrid = geneticSolve();
	//std::unique_ptr<cell[]> bestGrid = bruteForce();
	
	displayOptimalResult(bestGrid.get());
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

