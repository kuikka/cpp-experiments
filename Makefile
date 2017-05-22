CXXFLAGS=-std=c++14 -g -Og
LIBS=-latomic -lpthread

thread_pool: thread_pool.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f thread_pool thread_pool.o
