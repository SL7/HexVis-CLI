CC		:= gcc
CFLAGS	:= -Wall -Wextra -g

BIN		:= bin
SRC		:= src

LIBRARIES	:=

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= hexvis.exe
SOURCEDIRS	:= $(SRC)
else
EXECUTABLE	:= hexvis
SOURCEDIRS	:= $(shell find $(SRC) -type d)
endif

CINCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
CLIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
OBJECTS		:= $(SOURCES:.c=.o)

all: $(BIN)/$(EXECUTABLE)

.PHONY: clean
clean:
	-$(RM) $(BIN)/$(EXECUTABLE)
	-$(RM) $(OBJECTS)


run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(CINCLUDES) $(CLIBS) $^ -o $@ $(LIBRARIES)
