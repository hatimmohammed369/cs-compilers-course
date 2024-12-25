CC = g++
CFLAGS = -Wall -g -std=c++2a
LDFLAGS = -lm -lreadline

repl: main
	./main

norepl: main
	./main --file $(file)

main: main.o lexer.o parser.o
	$(CC) $(LDFLAGS) -o $@ $^

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

lexer.o: lexer.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

parser.o: parser.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
