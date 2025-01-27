TARGET = RUN
LIBS = -lm
CC = gcc
CFLAGS = -std=c98

.PHONY:	default all clean

default:	$(TARGET)
all:	default
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS:	$(TARGET) $(OBJECTS)

$(TARGET):	$(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:	-rm -f *.o core

cleanall:	clean
	-rm -f $(TARGET)
