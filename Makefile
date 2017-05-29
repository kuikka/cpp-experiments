CXXFLAGS=-std=c++14 -g -Og -W -Wall
LIBS=-latomic -lpthread

all: thread_pool main_loop

thread_pool: thread_pool.o
	$(CXX) -o $@ $^ $(LIBS)

main_loop: main_loop.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f thread_pool thread_pool.o
	rm -f main_loop main_loop.o
