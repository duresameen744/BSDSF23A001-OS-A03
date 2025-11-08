#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    printf("Readline test: if this compiles, readline is working!\n");
    char* input = readline("Test> ");
    if (input) {
        printf("You entered: %s\n", input);
        free(input);
    }
    return 0;
}
