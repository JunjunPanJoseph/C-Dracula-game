########################################################################
# COMP2521 18x1 // The Fury of Dracula // tests
#
# dryrun.view/Makefile: build tests for {Game,Hunter,Drac}View

CSE=1
CC	 = clang

CFLAGS	 = -Wall -Werror -std=c99 -g
CFLAGS	+= -fsanitize=address,undefined
CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-optimize-sibling-calls

LDFLAGS	 = -g
LDFLAGS	+= -fsanitize=address,undefined
LDFLAGS	+= -rdynamic -ldl

ifdef CSE
CC	 = clang-3.8
CFLAGS	+= -m32 -target i386-unknown-linux-gnu
LDFLAGS	+= -m32 -target i386-unknown-linux-gnu
endif

BINS	= testGameView testHunterView testDracView

all: ${BINS}

testGameView: testGameView.o GameView.o Map.o Places.o 
testGameView.o: testGameView.c Globals.h Game.h 

testHunterView: testHunterView.o HunterView.o GameView.o Map.o Places.o
testHunterView.o: testHunterView.c Map.c Places.h

testDracView: testDracView.o DracView.o GameView.o Map.o Places.o
testDracView.o: testDracView.c Map.c Places.h

Places.o: Places.c Places.h
Map.o: Map.c Map.h Places.h
GameView.o: GameView.c GameView.h structAndFun.h
HunterView.o: HunterView.c HunterView.h structAndFun.h
DracView.o: DracView.c DracView.h structAndFun.h

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core

.PHONY: prove
prove: testGameView testHunterView testDracView
	prove -f ./testGameView ./testHunterView ./testDracView