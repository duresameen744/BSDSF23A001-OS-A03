#include "shell.h"

int main() {
    char* cmdline;
    pipeline_t pipeline;

    // Initialize Readline if available
    initialize_readline();
    
    // Initialize job control
    init_jobs();
    
    // NEW: Initialize variables
    init_variables();

    while (1) {
        // Clean up zombie processes before prompt
        cleanup_zombies();
        update_jobs();

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
        
        // NEW: Handle variable assignments FIRST
        if (handle_variable_assignment(cmdline)) {
            // Variable was assigned, don't execute as command
            free(cmdline);
            continue;
        }
        
        // Handle control structures (if-then-else)
        if (is_if_then_else_command(cmdline)) {
            if_block_t if_block;
            if (parse_if_then_else(cmdline, &if_block) == 0) {
                execute_if_block(&if_block);
                free_if_block(&if_block);
            } else {
                fprintf(stderr, "Error: failed to parse if-then-else command\n");
            }
            free(cmdline);
            continue;
        }
        
        // Add non-empty commands to our internal history (after expansion)
        if (cmdline[0] != '\0' && cmdline[0] != '\n') {
            add_to_history(cmdline);
        }

        // Parse for redirection, pipes, and command chaining
        if (parse_redirection_pipes(cmdline, &pipeline) > 0) {
            // Execute the parsed command(s)
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
