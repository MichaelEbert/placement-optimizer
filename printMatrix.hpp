#pragma once
#include "gridManip.hpp"

template<typename T>
int printMatrix(T* matrix) noexcept
{
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			printf("%d\t", matrix[cell_linear_offset(j,i)]);
		}
		printf("\n");
	}
	return 0;
}

template<>
int printMatrix(float* matrix) noexcept
{
	printf("float\n");
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			printf("%f\t", matrix[cell_linear_offset(j,i)]);
		}
		printf("\n");
	}
	return 0;
}

template<>
int printMatrix(cell* matrix) noexcept
{
	char toPrint = 'x';
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			switch(matrix[cell_linear_offset(j,i)]){
				case NONE_ID:
					toPrint = '_';
					break;
				case REACTOR_ID:
					toPrint = 'R';
					break;
				case HEATSINK_ID:
					toPrint = 'S';
					break;
				case SPREADER_ID:
					toPrint = '+';
					break;
				case BOILER_ID:
					toPrint = 'B';
					break;
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

template<>
int printMatrix(LocalVars* matrix) noexcept
{
	printf("netPtr:\n");
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			printf("%p\t", matrix[cell_linear_offset(j,i)].netPtr);
		}
		printf("\n");
	}
	printf("localA:\n");
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			printf("%d\t", matrix[cell_linear_offset(j,i)].localA);
		}
		printf("\n");
	}
	printf("localB:\n");
	for(int i = 0; i < GRID_Y_SIZE; i++){
		for(int j = 0; j < GRID_X_SIZE; j++){
			printf("%d\t", matrix[cell_linear_offset(j,i)].localB);
		}
		printf("\n");
	}
	return 0;
}
