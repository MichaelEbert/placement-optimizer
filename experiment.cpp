#include <iostream>
#include <type_traits>
#include "fastlist.hpp"
#include <vector>
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
class Mouse{
	public:
	int spots;
	Mouse(){
		spots = 0;
		printf("default ctor\n");
	}
	Mouse(int spots){
		this->spots = spots;
		printf("spot ctor\n");
	}
	Mouse(Mouse const& other){
		printf("copy ctor\n");
		this->spots = other.spots;
	}
	Mouse(Mouse&& other){
		printf("move ctor\n");
		this->spots = other.spots;
	}
	//copy assignment
	Mouse& operator= (Mouse const& other){
		printf("copy assignment\n");
		this->spots = other.spots;
	}
	//move assignment
	Mouse& operator= (Mouse&& other){
		printf("move assignment\n");
		this->spots = other.spots;
	}
	
	~Mouse(){
		printf("mouse dtor called\n");
	}
	
	
	void speak() const{
		printf("I have %d spots!\n",spots);
	}
};

int main(int argc, char** argv){
	FastList<Mouse> list1;
	list1.add(3);
	list1.add(5);
	printf("list has %d elements\n",list1.curEntries);
	list1[0].speak();
	list1[1].speak();
	
	printf("\n\n----------\n\n");
	std::vector<Mouse> list2;
	list2.reserve(4);
	list2.emplace_back(2);
	list2.emplace_back(4);
	printf("list has %d elements\n",list2.size());
	list2[0].speak();
	list2[1].speak();
	printf("\n\n----------\n\n");
	//ensure it still works for normal numbers
	FastList<float> list3(4);
	list3.add(3.3);
	list3.add(1.76);
	printf("%f,%f\n",list3[0],list3[1]);
	return 0;
//	Cow c;
//	Wagon w;
//	Tractor t;
//	std::cout<<"cow:"<<countLegs(c);
//	std::cout<<"\nwagon:"<<countLegs(w);
//	std::cout<<"\ntractor:"<<countLegs(t);
}

