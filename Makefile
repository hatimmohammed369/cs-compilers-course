.PHONY: clean main

CC = g++
CFLAGS = -Wall -g -std=c++23
LDFLAGS = -lm -lreadline
HEADERS = *.hpp
EXECUTABLE = main

repl: clean main
	./main

norepl: clean main
	./main --file $(file)

main: object.o environment.o typing.o interpreter.o lexer.o syntax_tree.o parser.o main.o
	$(CC) $(LDFLAGS) -o $(EXECUTABLE) $^ $(HEADERS)
	chmod +x ./main

object.o: object.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

environment.o: environment.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

typing.o: typing.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

interpreter.o: interpreter.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

lexer.o: lexer.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

syntax_tree.o: syntax_tree.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

parser.o: parser.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f main *.o
