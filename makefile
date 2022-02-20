
NAME = G2MIDI
SRC := $(wildcard src/*.cpp)
INC_DIR = include/

LIB = -lrtmidi -ljack -lfftw3f -lm

all:
	g++ -o build/${NAME} -I ${INC_DIR} ${LIB} ${SRC}

clean:
	rm build/*