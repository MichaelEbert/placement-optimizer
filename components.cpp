//types of components available
//#include "components.hpp"



//static const void Reactor::component_setup(cell* thisCell, gsize_t x, gsize_t y){
//	unsigned char adjComponents[NUM_COMPONENT_TYPES];
//	sumAdjacentComponents(thisCell, x, y, adjComponents);
//	resource_grid[RES_ENERGY_ID][cell_linear_offset(x,y)] = adjComponents[REACTOR_ID] + 1;
//	resource_grid[RES_HEAT_ID][cell_linear_offset(x,y)] = adjComponents[REACTOR_ID] + 1;
//	//*thisCell=adjComponents[REACTOR_ID] + 1;//ticks
//	//this->adjComponents=adjComponents[HEATSINK_ID] + adjComponents[SPREADER_ID];
//}











//enum Resource {
//	HEAT,
//	POWER
//};
//
//typedef struct Effect{
//	Resource res;
//	int bonus;
//	int radius; //manhattan distance
//	int distribute;// 0 = add entire bonus to each affected component, 1 = split bonus across affected components
//	bool affectSelf;
//	unsigned short affected;
//} Effect;
//
//
//



/*reactor properties/rules:
generates 5 power.
generates 4 heat.(?)


PROBLEM: things can affect how much heat reactor generates.
so keep reactor heat internal until resolution step?
 steps:
 1)initialize and give modifiers (ex: reactor starts at 4 heat, reflector plate gives adjacent reactors 10% bonus)
 2)calculate distributed/cumulative effects (ex: dividing heat among adjacent components,???)
 3)(now heat is spawned on to components bordering reactor 
***it seems that this is a very hard problem that would require lots of processing to verify.
I should just use some parallelized filtering rules, and the ones that pass that get more extensively verified.

power rules:
only from reactors

heat rules:
never in reactors. passed to environment if no components next to it.

neighbor cell effect table:

cells	heat increase
1		+8
2		+12	
3		+16	
4		+20	

so for neighbor cell:
generate 5 power.
increase neighbor power by 5. (if reactor?)
increase neighbor heat by [TABLE] (if reactor)


*/

// #reactor:
//Component reactor = {
//	key  ='R';
//	effects = {{POWER,1,1,0,1,COMPONENT_REACTOR},
//		{res=HEAT,bonus=4,radius=1,distribute=true,affectSelf=false,affected=NOT(COMPONENT_REACTOR)}
//		}
//	}

