all: DVentry.o DV.o my-router.o my-router

DVentry.o:
	g++ -std=c++11 -c DVentry.cpp

DV.o: 
	g++ -std=c++11 -c DV.cpp

my-router.o:
	g++ -std=c++11 -c my-router.cpp

my-router:
	g++ -o my-router -std=c++11 -O2 my-router.o DV.o DVentry.o

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM my-router *.tar.gz

tarball: clean
	tar -cvf $(USERID).tar.gz *