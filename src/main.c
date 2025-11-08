#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;
    char* expanded_cmd = NULL;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
        // NEW: Handle history expansion before adding to history
        if (is_history_command(cmdline)) {
            expanded_cmd = expand_history_command(cmdline);
            if (expanded_cmd != NULL) {
                free(cmdline);
                cmdline = malloc(strlen(expanded_cmd) + 1);
                strcpy(cmdline, expanded_cmd);
                printf("%s\n", cmdline);  // Show the expanded command
            } else {
                free(cmdline);
                continue;  // Skip to next iteration if history expansion failed
            }
        }
        
        // NEW: Add non-empty commands to history (after expansion)
        if (cmdline[0] != '\0' && cmdline[0] != '\n') {
            add_to_history(cmdline);
        }

        if ((arglist = tokenize(cmdline)) != NULL) {
            // Check if it's a built-in command first
            if (handle_builtin(arglist) == 0) {
                // If not built-in, execute as external command
                execute(arglist);
            }

            // Free the memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
