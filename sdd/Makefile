CXX = g++

CXXFLAGS  += -std=gnu++11

APPS =  sdd

SRC=$(shell ls *.cpp)

all :  $(APPS) 

clean :
	rm -f *.o $(APPS)


dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MG $(SRC) >makefile.depend


.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<


sdd : main.o sdd_manager.o sdd_vtree.o sdd_node.o 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $^ -o $@

-include makefile.depend



