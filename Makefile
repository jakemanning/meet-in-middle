CXX=g++
INCLUDEBOOST=-I /usr/local/Cellar/boost/1.64.0_1/include
LIBBOOST=-L /usr/local/Cellar/boost/1.64.0_1/lib -l boost_timer -l boost_chrono -l boost_system
NTL=-lntl -lgmp -lm
OPTIONS=-g -O2 -std=c++11 -pthread -march=native $(NTL)
RM=rm -rf
FILES=meet_in_middle.cpp
OUTPUT=./meet_in_middle.o


all: compile run clean

compile:
	$(CXX) $(OPTIONS) $(FILES) -o $(OUTPUT)

run: 
	$(OUTPUT)

clean:
	$(RM) $(OUTPUT) *.dSYM
