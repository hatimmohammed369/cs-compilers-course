CC = g++
CFLAGS = -Wall -g -std=c++2a
LDFLAGS = -lm -lreadline

norepl: main
	./main --file $(file)

repl: main
	./main

main: main.o
	$(CC) $(LDFLAGS) -o $@ $^
	./main

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
