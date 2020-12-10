


all: test


test: test.cpp
	g++ $^ -o $@ ../src/modelling/algebraic/*.cpp -I ../include -I ../../HConLib/include -mavx -std=c++2a -lOpenImageIO ../src/visualization/*.cpp -L ../../HConLib/lib -lFlatAlg

