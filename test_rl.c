#include <readline/readline.h>
#include <stdio.h>

int main() {
    printf("Readline version: %s\n", rl_library_version);
    printf("rl_completion_ignore_case available: %s\n", 
#ifdef rl_completion_ignore_case
           "YES"
#else
           "NO"
#endif
    );
    return 0;
}
