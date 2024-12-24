#include <fstream>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>

#include "common.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Placeholder code: read it print it

    if (argc == 1) {
        // Interactive Mode
        // Read input from user directly

        // Line characters store
        char* buffer;
        // Nice object wrapper around line characters
        string line;
        while ((buffer = readline("> ")) != nullptr) {
            if (strlen(buffer) == 0) continue; // Ignore empty lines
            line = string(buffer);
            // add last read line to prompt history
            add_history(buffer);
            // free buffer because readline always allocates a new buffer
            free(buffer);
            // just print it
            cout << line << endl;
        }
    } else if (argc == 3 && (strcmp(argv[1], "--file") == 0 || strcmp(argv[1], "-f") == 0)) {
        // File Mode
        // Read input from file

        // Open requested file for reading
        ifstream input_file {argv[2]};
        // Seek to fil end
        input_file.seekg(0, std::ios::end);
        // Read total file size
        u64 file_size = input_file.tellg();
        // Allocate enough space to hold file contents
        char* input = new char[file_size];
        // Seek back to beginning of file to start contents copy
        input_file.seekg(0, std::ios::beg);
        // Read all characters
        input_file.read(input, file_size);
        printf("%s", input);

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
