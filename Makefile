all: DVnode.o DV.o my-router.o my-router

DVnode.o:
	g++ -std=c++11 -c DVnode.cpp

DV.o: 
	g++ -std=c++11 -c DV.cpp

my-router.o:
	g++ -std=c++11 -c my-router.cpp

my-router:
	g++ -o my-router -pthread -std=c++11 -O2 my-router.o DV.o DVnode.o

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM my-router *.tar.gz

tarball: clean
	tar -cvf $(USERID).tar.gz *