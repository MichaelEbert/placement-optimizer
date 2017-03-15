#include <cstdio>


int main(){
	int arr[]={0,2,3};
	for(int i = 0; i < 3; i++){
		switch(arr[i]){
		case 0:
			printf("zero");
			break;
		case 1:
			printf("one");
			break;
		case 2:
			printf("two");
			break;
		case 3:
			printf("three");
			break;
		default:
			printf("other");
			break;
		}
	}
	return 0;
}
