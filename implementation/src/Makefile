# For Linux (Ubuntu)
#
# Dependencies:
#  apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev
# (and clang too, or change CC to g++)
#
# On my system -DEXPERIMENTAL needs to be set for glew to expose "modern"
# OpenGL features. YMMV.

CC=clang++
CFLAGS=-Wall -std=c++11 -g -DDEBUG # -DEXPERIMENTAL

SRC=.
OUT=../build
GLM=../glm

LIBS=-lGL -lglut -lGLEW
INCLUDES=-I$(GLM)

sources = $(filter-out $(wildcard $(SRC)/assignment*),$(wildcard $(SRC)/*.cpp $(SRC)/*.c $(SRC)/*.C))
target_source := $(wildcard $(SRC)/$@.cpp $(SRC)/$@.c $(SRC)/$@.C)

all: assignment1.out

assignment1.out: $(wildcard $(SRC)/assignment1.cpp $(SRC)/assignment1.c $(SRC)/assignment1.C) $(sources) $(wildcard $(SRC)/*.hpp $(SRC)/*.h $(SRC)/*.H)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $(wildcard $(SRC)/assignment1.cpp $(SRC)/assignment1.c $(SRC)/assignment1.C) $(sources) -o $(OUT)/assignment1.out

clean:
	rm -f $(OUT)/*.out
