#pragma once
#include <cstdio>
#include "types.hpp"
//contains component definitions

//for REACTOR
//types:
//NONE
//REACTOR
//HEATSINK
//SPREADER
enum component_ids{
	NONE_ID,
	REACTOR_ID,
	HEATSINK_ID,
	SPREADER_ID,
	BOILER_ID
};

enum resource_ids{
	RES_ENERGY_ID,
	RES_HEAT_ID
};

class Component{
public:
	static const bool acceptsHeat = true;
	static const bool providesHeat = false;
	static const void component_setup(function_args& tlocals) noexcept;
	static const void component_action(function_args& tlocals) noexcept;	
};
class None : public Component{
	public:
		static const int index = NONE_ID;
		static const bool acceptsHeat = false;
		static const bool providesHeat = false;
		static const void component_setup(function_args& tlocals) noexcept;
};
/*
how reactors work:
localA:num adjacent heat acceptors
localB: heat to be dispersed to adjacent heat acceptors
setup: calculate heat/energy produced, get adjacent heat acceptors (for splitting heat)
run: nothing
*/
class Reactor: public Component{
	public:
	static const int index = REACTOR_ID;
	static const bool acceptsHeat = false;
	static const bool providesHeat = true;
	static constexpr int HEAT_PRODUCED = 20;
	static const void component_setup(function_args& tlocals) noexcept;
	static const void component_action(function_args& tlocals) noexcept;
};
/*
how heat sinks work:
localA:ResNet number?
setup: calculate heat produced
run: attach to heat networks?
*/
class HeatSink: public Component{
public:
	static const int index = HEATSINK_ID;
	static const bool acceptsHeat = true;
	static const bool providesHeat = false;
	static constexpr res_cell HEATSINK_HEAT_START = -1;
	static const void component_setup(function_args& tlocals) noexcept;
	static const void component_action(function_args& tlocals) noexcept;
};
/*
how spreaders work:
localA: ResNet number
setup: first draft ResNet
run:get adjacent heat, put onto me
*/
class Spreader : public Component{
public:
	static const int index = SPREADER_ID;
	static const bool acceptsHeat = true;
	static const bool providesHeat = true;
	static const void component_setup(function_args& tlocals) noexcept;
	static const void component_action(function_args& tlocals) noexcept;
};
/*
boiler: sells heat from adjacent hot things. will explode if exposed to heat.
*/
class Boiler : public Component{
public:
	static const int index = BOILER_ID;
	static const bool acceptsHeat = true;
	static const bool providesHeat = false;
	static const void component_setup(function_args& tlocals) noexcept;
	static const void component_action(function_args& tlocals) noexcept;
};


const component_func_t component_setup_arr[] = {
	None::component_setup,
	Reactor::component_setup,
	HeatSink::component_setup,
	Spreader::component_setup,
	Boiler::component_setup	
};

const component_func_t component_action_arr[] = {
	None::component_action,
	Reactor::component_action,
	HeatSink::component_action,
	Spreader::component_action,
	Boiler::component_action
};


template<class C>
constexpr component_properties static_props(){
	return {C::acceptsHeat,C::providesHeat};
}
static const component_properties static_properties[] = {
	static_props<None>(),
	static_props<Reactor>(),
	static_props<HeatSink>(),
	static_props<Spreader>(),
	static_props<Boiler>()
};
