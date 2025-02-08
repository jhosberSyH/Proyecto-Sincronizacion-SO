TARGET = RUN
LIBS = -lm -pthread
CC = gcc
CFLAGS = -std=c99 -pthread

.PHONY:	default all clean

default:	$(TARGET)
all:	default
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS:	$(TARGET) $(OBJECTS)

$(TARGET):	$(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:	-rm -f *.o core

cleanall:	clean
	-rm -f $(TARGET)
