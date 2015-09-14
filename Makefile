CC := gcc
CXX := g++
CFLAGS := -std=c99 -Wall -Wextra
CXXFLAGS := -std=c++11 -Wall -Wextra
# wasn't working for me LDFLAGS := ""
OBJS = iptree.o

all: gtest test_gtest shared_lib static_lib test_standalone

gtest: libgtest_main.a

libgtest_main.a: gtest/src/*.h gtest/src/*.cc gtest/include/gtest/*.h gtest/include/gtest/internal/*h
	g++ $(CXXFLAGS) -Igtest -Igtest/include gtest/src/gtest-all.cc gtest/src/gtest_main.cc -c 
	ar -rv $@ gtest-all.o gtest_main.o

test_gtest: $(OBJS) gtest test_iptree.cc
	g++ $(CXXFLAGS) $(OBJS) -pthread -Igtest/include test_iptree.cc libgtest_main.a -o $@ 

shared_lib: $(OBJS)
	gcc $(CFLAGS) -shared $(LDFLAGS) $(OBJS) -o libiptree.so

static_lib: $(OBJS)
	ar -rv libiptree.a $(OBJS)

test_standalone: $(OBJS)
	gcc $(CFLAGS) test.c $(OBJS) -o $@

.PHONY: clean

clean:
	rm -f *.o *.a *.so test_gtest test_standalone

