#include <iostream>
#include <type_traits>

typedef struct{
	const bool constLegs = false;
	int legs=5;
} cow;

typedef struct{
	const bool constLgs = true;
	int legs=3;
} wagon;

template<typename T>
int getLegs(T& thing){
	std::enable_if<true>
	return 4;
	return thing.legs;
}
int main(){
	cow c;
	wagon w;
	std::cout<<"cow:"<<getLegs<cow>(c);
	std::cout<<"\nwagon:"<<getLegs<wagon>(w);
	return 0;
}
