CC=ccache gcc
CFLAGS=-Wall -Wextra -O2

BIN=a.elf

HEADERDIR=.
SOURCEDIR=.
BUILDDIR=build


SOURCES= $(wildcard *.c)\
	$(wildcard src/*.c)\


OBJECTS= $(patsubst %.c, $(BUILDDIR)/%.o, $(notdir $(SOURCES)) )


all: bin run

# all: 
# 	@echo $(SOURCES)
# 	@echo $(OBJECTS)

nrun: bin

bin: $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJECTS)
	@echo "linking..."
	@$(CC) $(CFLAGS) -I$(HEADERDIR) -o $@ $^ -lm -lpthread

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@

$(BUILDDIR)/%.o: src/%.c
	@$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@

