TARGET = format_isql
LIBS = -lm
CC = g++
INCLUDES = -I.
CFLAGS = -std=c++0x -g -Wall $(INCLUDES)

.PHONY: clean all default

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cc, %.o, $(wildcard *.cc))
HEADERS = $(wildcard *.h)

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
