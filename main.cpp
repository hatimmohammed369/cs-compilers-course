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

        // Line characters store
        char* buffer;
        // Nice object wrapper around line characters
        string line;
        while ((buffer = readline("> ")) != nullptr) {
            if (strlen(buffer) == 0) continue; // Ignore empty lines
            line = string(buffer);
            add_history(buffer); // add last read line to prompt history
            free(buffer); // free buffer because readline always allocates a new buffer
            cout << line << endl; // just print it
        }
    } else if (argc == 3 && (strcmp(argv[1], "--file") == 0 || strcmp(argv[1], "-f") == 0)) {
        // Read input from file
        ifstream input_file {argv[2]};
        input_file.seekg(0, std::ios::end);
        u64 file_size = input_file.tellg();
        char* input = new char[file_size];
        input_file.seekg(0, std::ios::beg);
        input_file.read(input, file_size);
        printf("%s", input);
    } else {
        cerr << "Invalid command-line arguments\n" ;
        cerr << "Usage:\n" ;
        cerr << "   ./main\n" ;
        cerr << "   ./main (--file/-f) path\n" ;
    }
    return 0;
}
