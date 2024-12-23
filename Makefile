CC = g++
CFLAGS = -Wall -g -std=c++17
LDFLAGS = -lm -lreadline

run : main
	./main

main : main.o
	$(CC) $(LDFLAGS) -o $@ $^
	./main

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
