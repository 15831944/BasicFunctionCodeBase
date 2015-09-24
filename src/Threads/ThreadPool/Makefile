CFLAGS = -D_REENTRANT -Wall -pedantic -I.
LDLIBS = -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = tests/thrdTest tests/heavyTest tests/shutdownTest

all: $(TARGETS)

tests/shutdownTest: tests/shutdownTest.o ThreadPool.o
tests/thrdTest: tests/thrdTest.o ThreadPool.o
tests/heavyTest: tests/heavyTest.o ThreadPool.o

ThreadPool.o: ThreadPool.c ThreadPool.h
tests/thrdTest.o: tests/thrdTest.c ThreadPool.h
tests/heavyTest.o: tests/heavyTest.c ThreadPool.h

clean:
	rm -f $(TARGETS) *~ */*~ */*.o
