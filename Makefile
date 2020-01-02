FLAGS = -Wall -Werror

solver : solver.cpp
	g++ ${FLAGS} -o $@ $^

debug : solver.cpp
	g++ -g ${FLAGS} -o solver $^
	
clean : 
	rm *.o solver
