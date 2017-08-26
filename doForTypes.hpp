#pragma once
#include "components.hpp"
//call to run a functor over all component types.
//use: 
//create a functor following this format:
/*
template<class ComponentType>
struct $NAME{
	$YOUR_FUNCTION_HERE
};
*/
//NOTE: the function $YOUR_FUNCTION_HERE has 2 restrictions:
//1)it MUST be named 'func'
//2)it can a maximum of 1 argument.

// It will be called once for every component type.
// you can access the component type through ComponentType.

//after you create your functor, here's how to use it:
/*
FoldOverAllTypes<$RETURN_TYPE,$NAME,$ARGUMENT_TYPE,FoldAdd>(arg);
*/
//where $RETURN_TYPE is the return type of your function, 
//$ARGUMENT_TYPE is the argument type of your function (void if none),
//and arg is the argument you want to pass in (if any).
//FoldAdd will add the results. Other options coming if I need them.

//---code---
//targetFunctor = functor to execute on each element. listOp = how to concatenate each element
template<typename RetType, template<class> typename targetFunctor, typename ArgType, template<typename,typename,template<class>typename,class...> typename listOp>
constexpr RetType DoForAllTypes(){
	return listOp<RetType,ArgType,targetFunctor,None,Spreader,HeatSink,Reactor,Boiler>::func();
}

//with single argument
template<typename RetType, template<class> typename targetFunctor, typename ArgType, template<typename,typename,template<class>typename,class...> typename listOp>
constexpr RetType DoForAllTypes(ArgType arg){
	return listOp<RetType,ArgType,targetFunctor,None,Spreader,HeatSink,Reactor,Boiler>::func(arg);
}

//---add concatenator---
template<typename ResultType,typename ArgType, template<class> class Functor, class... Types>
struct FoldAdd;

//zero-argument specialization
template<typename ResultType, template<class> class Functor, class First, class... Types>
struct FoldAdd<ResultType,void,Functor,First,Types...>{
	static ResultType func(){
			return Functor<First>::func()+FoldAdd<ResultType,void,Functor,Types...>::func();
	}
};
template<typename ResultType, template<class> class Functor, class Penultimate, class Last>
struct FoldAdd<ResultType,void,Functor,Penultimate,Last>{
	static ResultType func(){
		return Functor<Penultimate>::func()+Functor<Last>::func();
	}
};

////one-argument specialization
template<typename ResultType, typename ArgType, template<class> class Functor, class First, class... Types>
struct FoldAdd<ResultType,ArgType,Functor,First,Types...>{
	static ResultType func(ArgType arg){
			return Functor<First>::func(arg)+FoldAdd<ResultType,ArgType,Functor,Types...>::func(arg);
	}
};

template<typename ResultType, typename ArgType, template<class> class Functor, class Penultimate, class Last>
struct FoldAdd<ResultType,ArgType,Functor,Penultimate,Last>{
	static ResultType func(ArgType arg){
		return Functor<Penultimate>::func(arg)+Functor<Last>::func(arg);
	}
};

//---ensure the number of types is kept up-to-date---
namespace FoldValidation{
template<typename ResultType,typename ArgType, template<class> class Functor, class First, class... Types>
struct CountNumTypes{static constexpr int func(){return 1+sizeof...(Types);}};
template<typename ComponentType> struct cnt{constexpr int func(){return 0;}};
static_assert(DoForAllTypes<int,cnt,void,CountNumTypes>() == NUM_COMPONENT_TYPES,"FoldOverAllTypes() has an incorrect amount of types");
}
