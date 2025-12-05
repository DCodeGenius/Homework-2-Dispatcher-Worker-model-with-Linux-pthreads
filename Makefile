CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pthread

OBJS = hw2.o queue.o worker.o parser.o counters.o stats.o timing.o logging.o

all: hw2

hw2: $(OBJS)
	$(CC) $(CFLAGS) -o hw2 $(OBJS)

hw2.o: hw2.c common.h queue.h worker.h counters.h stats.h timing.h logging.h parser.h
queue.o: queue.c queue.h common.h
worker.o: worker.c worker.h common.h queue.h counters.h stats.h timing.h logging.h parser.h
parser.o: parser.c parser.h common.h
counters.o: counters.c counters.h common.h
stats.o: stats.c stats.h common.h timing.h
timing.o: timing.c timing.h
logging.o: logging.c logging.h common.h timing.h

clean:
	rm -f *.o hw2
