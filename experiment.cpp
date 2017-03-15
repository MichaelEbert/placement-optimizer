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

int main(){
	Cow c;
	Wagon w;
	Tractor t;
	std::cout<<"cow:"<<countLegs(c);
	std::cout<<"\nwagon:"<<countLegs(w);
	std::cout<<"\ntractor:"<<countLegs(t);
}

