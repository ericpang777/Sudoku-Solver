CXX = g++
CXXFLAGS = -Wall -Werror

solver: solver.o avail_nums.o
	$(CXX) $(CXXFLAGS) -o solver solver.o avail_nums.o

solver.o: solver.cpp avail_nums.h
	$(CXX) $(CXXFLAGS) -c solver.cpp

avail_nums.o: avail_nums.cpp avail_nums.h
	$(CXX) $(CXXFLAGS) -c avail_nums.cpp