//gridManip.h - contains grid manipulation methods
#include <cstring>

inline bool is_left_edge(gsize_t index){
	return (index%GRID_X_SIZE != 0);
}
inline bool is_right_edge(gsize_t index){
	return ((index+1)%GRID_X_SIZE != 0);
}
inline bool is_top_edge(gsize_t index){
	return (index < GRID_X_SIZE);
}

inline bool is_bottom_edge(gsize_t index){
	return (index >= ((GRID_Y_SIZE-1)*GRID_X_SIZE));
}

inline goffset_t cell_XOffset(goffset_t deltaX){
	return deltaX;//*std::static_cast<goffset_t>(sizeof cell)
}
inline goffset_t cell_YOffset(goffset_t deltaY, gsize_t xSize){
	return deltaY* (static_cast<goffset_t>(xSize));//*std::static_cast<goffset_t>(sizeof cell)
}

inline gsize_t cell_linear_offset(gsize_t x,gsize_t y){
	return x+(y*GRID_X_SIZE);
}

cell* cell_neighbor(cell* thisCell, goffset_t deltaX, goffset_t deltaY){
	return thisCell + cell_XOffset(deltaX) + cell_YOffset(deltaY, GRID_X_SIZE);
}

//sums the resources of type RESOURCE for grid GRID
int sum_grid(grid grid, gsize_t gridSize,  unsigned int resource){
	int total = 0;
	//cell* resGrid = grid+(gridSize * resource);
	for(gsize_t i = 0; i < gridSize; i++){
		total+= static_cast<signed char>(grid[i]);
	}
	return total;
}

int setup_grid(grid typegrid, gsize_t gridSize){
	for(gsize_t i = 0; i < gridSize; i++){
		component_setup_arr[typegrid[i]](typegrid+i, i%GRID_X_SIZE, i/GRID_X_SIZE);
	}
}

//
void increment_grid(grid grid, gsize_t gridSize){
	grid[0]++;
	//if grid[0] == NUM_COMPONENT_TYPES, carry
	for(gsize_t i = 0; i < gridSize; i++){
		if(grid[i] == NUM_COMPONENT_TYPES){
			grid[i] = 0;
			grid[i+1]++;
		}
		else{
			return;
		}
	}
}

//need to fix out of bounds errors: x,y = -1 or maxX+1
void sumAdjacentComponents(cell* thisCell, gsize_t x, gsize_t y, unsigned char* adjComponents)
{
	memset(adjComponents,0, sizeof(adjComponents));
	cell* adjCell;
	for(int i = 0; i < 4; i++){
		bool invalid = false;
		switch(i){
			case 0:
			adjCell = cell_neighbor(thisCell, 1,0);
			invalid = (x >= GRID_X_SIZE-1);
			break;
			case 1:
			adjCell = cell_neighbor(thisCell, -1,0);
			invalid = (x <= 0);
			break;
			case 2:
			adjCell = cell_neighbor(thisCell, 0,1);
			invalid = (y >= (GRID_Y_SIZE-1));
			break;
			case 3:
			adjCell = cell_neighbor(thisCell, 0,-1);
			invalid = (y <= 0);
			break;
		}
		if(!invalid){
			adjComponents[*adjCell]++;
		}
	}
	return;
}

