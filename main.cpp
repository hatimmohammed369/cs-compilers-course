#include <fstream>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>

#include "interpreter.hpp"
#include "object.hpp"
#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // Placeholder code: read it print it
    Parser parser;
    Interpreter interpreter;
    ParseResult result;
    if (argc == 1) {
        // Interactive Mode
        // Read input from user directly
        *Config::get_mode() = Mode::Interactive;
        InterpreterResult eval;
        // Line characters store
        char* buffer;
        while ((buffer = readline("> ")) != nullptr) {
            if (strlen(buffer) == 0) continue; // Ignore empty lines
            // add last read line to prompt history
            add_history(buffer);
            parser.init(buffer, strlen(buffer));
            result = parser.parse_source();
            if (result.is_ok()) {
                eval = interpreter.interpret(result.unwrap());
                if (eval.is_usable())
                    cout << eval.unwrap() << '\n' ;
                else if (eval.is_error())
                    cerr << eval.unwrap_error() << '\n' ;
            } else {
                cerr << parser.errors() << " syntax errors found\n";
            }
            // free buffer because readline always allocates a new buffer
            free(buffer);
        }
    } else if (argc == 3 && (strcmp(argv[1], "--file") == 0 || strcmp(argv[1], "-f") == 0)) {
        // File Mode
        // Read input from file
        *Config::get_mode() = Mode::File;
        // Open requested file for reading
        ifstream input_file {argv[2]};
        // Seek to fil end
        input_file.seekg(0, std::ios::end);
        // Read total file size
        u64 file_size = input_file.tellg();
        // Allocate enough space to hold file contents
        char* input = new char[file_size + 1];
        input[file_size] = '\0';
        // Seek back to beginning of file to start contents copy
        input_file.seekg(0, std::ios::beg);
        // Read all characters
        input_file.read(input, file_size);
        parser.init(input, file_size);
        result = parser.parse_source();
        if (result.is_ok()) {
            InterpreterResult r = interpreter.interpret(result.unwrap());
            if (r.is_error())
                cerr << r.unwrap_error() << '\n' ;
        } else {
            cerr << parser.errors() << " syntax errors found\n";
        }
        // Free input buffer
        delete[] input;
    } else {
        // Print help on how to use
        cerr << "Invalid command-line arguments\n" ;
        cerr << "Usage:\n" ;
        cerr << "   ./main\n" ;
        cerr << "   ./main (--file/-f) path\n" ;
    }
    return 0;
}
