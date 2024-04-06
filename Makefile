CC = gcc -Wall -Wvla -std=c99 -fsanitize=address
CFLAGS=-I.
OBJ = mysh.c

%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

mysh: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
