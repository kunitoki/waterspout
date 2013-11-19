
IDIR=../include
ODIR=.
LDIR=../lib

CXX=g++
CFLAGS=-march=native -ggdb -I$(IDIR)
LIBS=-lm

_DEPS=waterspout.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o test
