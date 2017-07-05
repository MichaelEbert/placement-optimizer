placement optimizer:

layout(aka "why is everything in header files?"):
idk man. 
aight this is all fucked. best case scenario i can write a generator to produce the 
cpp files from some other source.
I want to have data separated by resource, not component.
 This allows for the cache to hold more of 1 type grid. I wonder if this is worth it -- how
 often do I only access 1 grid vs multiple? 
 grid-by-type allows simd to work better too - as long as im using the same grid.
 
 performance vs readability: how much perf is needed? i mean, max perf is preferrable, but well-written c++... 
 holy shit cramming everything into main.cpp incrases default performance by 33%. -flto DECREASES performance (???)
