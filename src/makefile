# location of header files
IDIR = ../include

# compiler to use
CC = gcc

# compiler flags
CP = -Wall
CFLAGS = -I$(IDIR) -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

# header dependencies
_DEPS = initShader.h matrixOperations.h geometricShapes.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
ODIR = obj
_OBJ = initShader.o matrixOperations.o geometricShapes.c
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: pool

pool: pool.c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(CP)

$(ODIR)/%.o: %.c $(IDIR)/%.h
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
