#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>

using namespace std;

int main() {
    // Placeholder code: read it print it

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
    return 0;
}
