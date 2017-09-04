#pragma once
#include "resourceNetwork.hpp"
const short NUM_COMPONENT_TYPES = 5;
const short NUM_RESOURCE_TYPES = 2;
const short GRID_X_SIZE = 4;
const short GRID_Y_SIZE = 3;

const int GRID_SIZE = GRID_X_SIZE*GRID_Y_SIZE;
typedef unsigned char cell;
typedef unsigned int gsize_t;
typedef signed long goffset_t;//max offset = entire grid.
typedef signed int result_t;//type of result used in genetic algorithm to evaluate candidates

typedef uint8_t adjacency_t;//type of adjacency special grid
typedef float res_cell;

#include "endpoint.hpp" //fuck this i'll fix it later
//GRID TYPEDEFS
//basically instead of doing (instance.field), we want to write field[instance]
//grids: adjacency special grid, resource grids, property grid 

typedef struct {
	bool acceptsHeat;
	bool providesHeat;
	bool networkable;
} component_properties;

//localA is for variables that represent something for every type. localB is for whatever scrap you want
typedef struct {
	//uint8_t adjComponents[NUM_COMPONENT_TYPES];
	union{
		struct{
			uint8_t Prop1:1;
			uint8_t localA;
			uint8_t localB;
			uint8_t localC;
		};
		void* ptrB;
	};
	void* netPtr;
} LocalVars;


//FUNCTION TYPES
//this struct contains thread local vars
typedef struct{
	gsize_t thisCell;
	cell* typegrid;
	adjacency_t* adjacency_sg;
	res_cell* energy_g;
	res_cell* heat_g;
	LocalVars* locals_g;
	ResourceNetworkManager<res_cell> resNet;
	std::vector<Endpoint> endpointList;
} function_args;

//most functions should take in all variables for use... huh. think about inlining more maybe?
//function example declaration: func(gsize_t thisCell, grid typegrid, adjacency_t* adjacency_sg)
typedef const void (*component_func_t)(function_args&);

#define COUNT_SIMS true
const unsigned long ONE_BILLION = 1000000000;
unsigned long count_sims_low = 0;
unsigned long count_sims_high = 0;


//misc helper functions
template <typename T, size_t N>
constexpr size_t array_size(T(&)[N]) {
	return N;
}

template<typename T>
constexpr T const_pow(T base, T exp) {
	return exp == 0 ? 1 : base * const_pow(base, exp - 1);
}
