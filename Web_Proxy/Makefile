CXX = gcc
CXXFLAGS = -Wall -Wextra -std=c11 -pthread -lssl -lcrypto

OBJECTS =  myproxy.o

myproxy: $(OBJECTS)
	$(CXX) -g $(CXXFLAGS) -o bin/myproxy myproxy.o
    
myproxy.o: src/myproxy.c src/myproxy.h
	$(CXX) -g $(CXXFLAGS) -c src/myproxy.c


clean: 
	rm -f *.o
	rm myproxy 

