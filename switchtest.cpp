#include <cstdio>
#include "resourceNetwork.hpp"


typedef struct{
	Network myNet;
} Cell;

Cell cellA,cellB,cellC;

void printNetworks(){
	printf("\
	Network Names:\n\
	A:%X\n\
	B:%X\n\
	C:%X\n\
	Network Values:\n\
	A:%X\n\
	B:%X\n\
	C:%X"
	,*cellA.myNet,*cellB.myNet,*cellC.myNet,**cellA.myNet,**cellB.myNet,**cellC.myNet);
}

int main(){
	ResourceNetwork resNet;
	cellA.myNet = resNet.newNetwork();
	cellB.myNet = resNet.newNetwork();
	cellC.myNet = resNet.newNetwork();
	
	**(cellA.myNet)=5;
	**(cellB.myNet)=3;
	
	resNet.joinNetworks(cellC.myNet,cellA.myNet);
	printNetworks();
}
