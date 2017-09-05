#pragma once
#include <type_traits>
#include <cstring>
#include <cstdlib>
//vector<> code takes an absurd amount of time to run, so I get to roll my own.


//std::vector<Network<T> > valueNetworkLists[MAX_NETWORKS];
//
//
//Network<T> ResourceNetworkManager<T>::newNetwork() {
//	if (curMaxNet >= MAX_NETWORKS) {
//		printf("error max networks reached\n");
//		return nullptr;
//	}
//	networks[curMaxNet] = &values[curMaxNet];
//	valueNetworkLists[curMaxNet].emplace_back(&networks[curMaxNet]);
//	numActualNets++;
//	return &networks[curMaxNet++];
//}
//
//template<typename T>
//void ResourceNetworkManager<T>::joinNetworks(Network<T> a,Network<T> b){
//	if(a == nullptr || b == nullptr || *b==*a){
//		return;
//	}
//	
//	auto bValueIndex = (*b)-values;
//	auto aValueIndex = (*a)-values;
//	auto& listA = valueNetworkLists[aValueIndex];
//	auto& listB = valueNetworkLists[bValueIndex];
//	for(auto net:listB){
//		*net = *a;
//	}
//	listA.insert(listA.end(),listB.begin(),listB.end());
//	numActualNets--;
//}
//
////reinitialize for new map
//template<typename T>
//void ResourceNetworkManager<T>::reset(){
//	for(int i = 0; i < curMaxNet-1;i++){
//		valueNetworkLists[i].clear();
//	}
//	for(int i = 0; i < MAX_NETWORKS; i++){
//		values[i] = 0;
//	}
//	curMaxNet=0;
//	numActualNets=0;
//	return;
//}

//what do we need this to do?
//valueNetworkLists: an array of T** lists
//in init:
//	put i'th Network into netlists[i]
//	
//in join:
//	(let new point to newval)
//	for every Network in netlists[old]:
//		point the network to newval
//	add all of netlists[old] to netlists[new]
//	
//in reset:
//	reset all
	
//good at merging a few elements
//capable of merging many elements if needed, most merges will probably 1+1 or n+1.
//max netlist size is MAX_NETWORKS


/*
--OPTIONS--
1) array of [network, valAddr]. search through entire list for every join.
	1a) array of [network,valAddr] combined with array of [valAddr,numNets]. search thru until numNets changed.
2) array of [valAddr,{netList}]
-dynamically allocated list
-some sorta hybrid linked list
3) array of size MAX_NETS. each pointsHere is represented by a (startVal,numEntries). can swap 1 val, then just increment startVal?no, may have to increment ALL startVals.

vector to join:
(get new valAddr)
(get ptsHere[i])

get ptr to ptsHere entries
for each entry: dereference entry
	set entry' to newValAddr
	check to see if last entry
	
does newPtsHere have enough space?(totalSpace - numEntries >= numOldEntries)
no: malloc() new area
	memcpy curdata to new area
	set new totalSpace
	set new entryPtr
yes:memcpy block to newPtsHere
	add N to numEntries



idea: allocate some large amount of memory, do own
memory mgmt.
b/c 2 entries merging takes same amt of room, 
just need to shuffle some mem.
-but how do we shuffle mem? ptrs?

so to join:
(get new valAddr)
(get ptsHere[i])

for each entry:
	dereference entry
	set entry' to newValAddr
	check to see if last entry

does ptsHere[new] have enough space? (ptsHere[new] > n?
get ptr to ptsHereList
...
so this uses more operations (possibly adding N to all ptrs) -- use if need less mem?
*/

//like vector, but a skeletal subset of functionality and only for trivial types. no deletion for one.
template<typename T>
class FastList{
//static_assert(std::is_trivial<T>::value,"fastlist is only fast for trivial types; it doesn't implement references and stuff for construction.");
public:
	static const int defaultStartingEntries = 4;
	
	unsigned short curEntries;
	FastList();
	FastList(int const numStartingEntries);
	FastList(FastList<T>&& other);
	FastList(FastList<T> const& other) = delete;
	FastList& operator= (FastList<T>&& other);
	FastList& operator= (FastList<T> const& other) = delete;
	T const& operator[](unsigned short const i){
		return entryArray[i];
	}
	template<typename... Arg>
	void emplace(Arg&&... arg);
	void add(T const element);
	void addAll(FastList<T>& lst);
	void clear();
	
	T* begin(){
		return entryArray;
	}
	T* end(){
		return entryArray+curEntries;
	}
	
	~FastList();
private:
	unsigned short 	maxEntries;
	T* entryArray;
};

template<typename T>
FastList<T>::FastList(){
	curEntries = 0;
	maxEntries = FastList::defaultStartingEntries;
	entryArray = static_cast<T*>(malloc(sizeof(T)*FastList::defaultStartingEntries));
	return;
}

template<typename T>
FastList<T>::FastList(int const numStartingEntries) {
	curEntries = 0;
	maxEntries = numStartingEntries;
	entryArray = static_cast<T*>(malloc(sizeof(T)*numStartingEntries));
	return;
}

template<typename T>
FastList<T>::FastList(FastList<T>&& other) {
	curEntries = other.curEntries;
	maxEntries = other.maxEntries;
	entryArray = other.entryArray;
	other.entryArray = nullptr;
	return;
}
template<typename T>
FastList<T>& FastList<T>::operator= (FastList<T>&& other) {
	curEntries = other.curEntries;
	maxEntries = other.maxEntries;
	entryArray = other.entryArray;
	other.entryArray = nullptr;
	return *this;
}

template<typename T>
FastList<T>::~FastList(){
	free(entryArray);
	return;
}

template<typename T>
void FastList<T>::add(T const element){
	//printf("adding entry\n");
	if(this->maxEntries == this->curEntries){
		entryArray = static_cast<T*>(realloc(entryArray, 2*maxEntries));
		this->maxEntries *= 2;
	}
	entryArray[this->curEntries] = element;
	this->curEntries++;
	return;
}

template<typename T>template<typename... Arg>
void FastList<T>::emplace(Arg&&... arg){
	//printf("adding entry with forwarding\n");
	if(this->maxEntries == this->curEntries){
		entryArray = static_cast<T*>(realloc(entryArray, 2*maxEntries));
		this->maxEntries *= 2;
	}
	T* thisEntry = entryArray+this->curEntries;
	new(thisEntry) T(std::forward<Arg>(arg)...);
	this->curEntries++;
	return;
}


template<typename T>
void FastList<T>::addAll(FastList<T>& lst){
	if(this->maxEntries - this->curEntries < lst.curEntries){
		if(lst.curEntries > maxEntries){
			entryArray = static_cast<T*>(realloc(entryArray, 2* lst.curEntries));
			this->maxEntries = 2*lst.curEntries;
		}
		else{
			entryArray = static_cast<T*>(realloc(entryArray, 2*maxEntries));
			this->maxEntries *= 2;
		}
	}
	if(std::is_trivial<T>::value){
		memcpy(entryArray+this->curEntries, lst.entryArray, sizeof(T)*lst.curEntries);
		this->curEntries += lst.curEntries;
	}
	else{
		for(int i = 0; i < lst.curEntries; i++){
			entryArray[this->curEntries] = std::move(lst[i]);
			this->curEntries++;
		}
	}
	return;
}

//doesn't currently deallocate memory.
template<typename T>
void FastList<T>::clear(){
	this->curEntries = 0;
	if(std::is_trivial<T>::value){
		memset(entryArray,0,sizeof(T)*curEntries);
	}
	else{
		//this doesn't work for nontrivial types.
		return;
	}
}
