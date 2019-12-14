CXX=g++
CXXFLAGS=$(shell pkgconf --cflags sfml-all) -O2 -std=c++17
LDFLAGS=$(shell pkgconf --libs sfml-all)
OBJS=main.o board.o panel.o
BIN=minesweeper

.PHONY: clean

${BIN}: ${OBJS}
	${CXX} ${CXXFLAGS} ${LDFLAGS} $^ -o $@

clean:
	rm *.o ${BIN} || true
