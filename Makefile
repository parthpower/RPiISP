TARGET=rpiprog
CC=gcc
CFLAGS=-g 
LIBS= -lm -lbcm2835
SOURCEDIR=src
BUILDDIR=build

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(BUILDDIR)/$(TARGET)

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC)  $^ -o $@ $(LIBS)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	-rm -f $(BINDIR)/*.o
	-rm -f $(BUILDDIR)/$(TARGET)
