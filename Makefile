.PHONY: clean main

CC = g++
CFLAGS = -Wall -g -std=c++2a
LDFLAGS = -lm -lreadline
HEADERS = *.hpp
EXECUTABLE = main

repl: clean main
	./main

norepl: clean main
	./main --file $(file)

main: main.o lexer.o syntax_tree.o parser.o
	$(CC) $(LDFLAGS) -o $(EXECUTABLE) $^ $(HEADERS)
	chmod +x ./main

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

lexer.o: lexer.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

syntax_tree.o: syntax_tree.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

parser.o: parser.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f main *.o
