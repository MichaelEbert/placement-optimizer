#include <iostream>
#include <type_traits>


template<typename T>
typename std::enable_if<T::constLegs,int>::type countLegs(T& t){
	return 4;
}

template<typename T>
typename std::enable_if<!T::constLegs,int>::type countLegs(T& t){
	return t.legs;
}


//DEFAULTS
class FarmThing{
	public:
	static const bool constLegs = true;
};

class Tractor :public FarmThing{
	public:
	int legs = 12;
};

class Cow :public FarmThing{
};

class Wagon:public FarmThing{
	public:
	int legs = 6;
	static const bool constLegs=false;
};
//scratch
class Endpoint {
	public:
	uint32_t attachedNetworks;
	float unitsAccepted;
	Endpoint(float unitsAccepted, uint32_t attachedNets){
		this->unitsAccepted = unitsAccepted;
		this->attachedNetworks = attachedNets;
	}
};

struct EndStruct{
	uint32_t an;
	float ua;
};

int main(int argc, char** argv){
	printf("%d,%d",sizeof(Endpoint),sizeof(EndStruct));
	return 0;
//	Cow c;
//	Wagon w;
//	Tractor t;
//	std::cout<<"cow:"<<countLegs(c);
//	std::cout<<"\nwagon:"<<countLegs(w);
//	std::cout<<"\ntractor:"<<countLegs(t);
}

