CC = gcc
CFLAGS = -Wall -g
TARGET = program
OBJS = main.o ls.o clear.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

ls.o: ls.c header.h tree_io.h
	$(CC) $(CFLAGS) -c $< -o $@

clear.o: clear.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:	
	rm -f $(TARGET) $(OBJS)