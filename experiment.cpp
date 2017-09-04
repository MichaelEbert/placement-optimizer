#include <iostream>
#include <type_traits>
#include "fastvector.hpp"

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
//test if double ptr deref is different from ptr+offset


int main(int argc, char** argv){
	FastList<float> list1;
	list1.add(4);
	list1.add(4.3);
	list1.add(8.5);
	list1.add(list1[1]);
	FastList<float> list2;
	list2.add(-2);
	list2.addList(list1);
	list2.add(-5);
	printf("%f,%f,%f,%f\n",list1[0],list1[1],list1[2],list1[3]);
	printf("%f,%f,%f,%f,%f,%f\n",list2[0],list2[1],list2[2],list2[3],list2[4],list2[5]);
	return 0;
//	Cow c;
//	Wagon w;
//	Tractor t;
//	std::cout<<"cow:"<<countLegs(c);
//	std::cout<<"\nwagon:"<<countLegs(w);
//	std::cout<<"\ntractor:"<<countLegs(t);
}

