#include "shell.h"

int main() {
    char* cmdline;
    pipeline_t pipeline;

    // Initialize Readline if available
    initialize_readline();

    while (1) {
        // Use readline if available, otherwise fallback
        cmdline = read_cmd_readline(PROMPT);
        
        if (cmdline == NULL) {
            break; // EOF (Ctrl+D)
        }

        // Handle history expansion before adding to our internal history
        if (is_history_command(cmdline)) {
            char* expanded_cmd = expand_history_command(cmdline);
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
        
        // Add non-empty commands to our internal history (after expansion)
        if (cmdline[0] != '\0' && cmdline[0] != '\n') {
            add_to_history(cmdline);
        }

        // NEW: Parse for redirection and pipes
        if (parse_redirection_pipes(cmdline, &pipeline) > 0) {
            // Execute the parsed command(s) - removed unused 'result' variable
            execute_pipeline(&pipeline);
            
            // Free allocated memory
            free_pipeline(&pipeline);
        } else {
            fprintf(stderr, "Error: failed to parse command\n");
        }
        
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
