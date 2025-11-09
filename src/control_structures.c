#include "shell.h"

// Simple approach: Handle if-then-else as single line commands

// Check if command is an if-then-else statement
int is_if_then_else_command(const char* cmdline) {
    if (cmdline == NULL) return 0;
    
    // Look for the pattern: if ... then ... fi
    return (strstr(cmdline, "if ") != NULL && 
            strstr(cmdline, " then ") != NULL && 
            strstr(cmdline, " fi") != NULL);
}

// Parse if-then-else from a single line
int parse_if_then_else(const char* cmdline, if_block_t* if_block) {
    if (cmdline == NULL || if_block == NULL) {
        return -1;
    }
    
    // Initialize if_block
    if_block->condition = NULL;
    if_block->then_count = 0;
    if_block->else_count = 0;
    if_block->has_else = 0;
    
    for (int i = 0; i < MAX_BLOCK_LINES; i++) {
        if_block->then_commands[i] = NULL;
        if_block->else_commands[i] = NULL;
    }
    
    char* copy = strdup(cmdline);
    
    // Find the positions of key words
    char* if_pos = strstr(copy, "if ");
    char* then_pos = strstr(copy, " then ");
    char* else_pos = strstr(copy, " else ");
    char* fi_pos = strstr(copy, " fi");
    
    if (if_pos == NULL || then_pos == NULL || fi_pos == NULL) {
        free(copy);
        return -1;
    }
    
    // Extract condition (between "if " and " then ")
    *then_pos = '\0'; // Terminate at "then"
    if_block->condition = strdup(if_pos + 3); // Skip "if "
    
    // Extract then commands (between " then " and " else " or " fi")
    char* then_start = then_pos + 6; // Skip " then "
    
    if (else_pos != NULL) {
        *else_pos = '\0';
        if_block->has_else = 1;
    } else {
        *fi_pos = '\0';
    }
    
    // Parse then commands
    char* then_commands = then_start;
    char* token = strtok(then_commands, ";");
    while (token != NULL && if_block->then_count < MAX_BLOCK_LINES) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        if (strlen(token) > 0) {
            if_block->then_commands[if_block->then_count++] = strdup(token);
        }
        token = strtok(NULL, ";");
    }
    
    // Extract else commands if present
    if (if_block->has_else && else_pos != NULL) {
        char* else_start = else_pos + 6; // Skip " else "
        *fi_pos = '\0'; // Terminate at "fi"
        
        // Parse else commands
        char* else_commands = else_start;
        token = strtok(else_commands, ";");
        while (token != NULL && if_block->else_count < MAX_BLOCK_LINES) {
            // Trim whitespace
            while (*token == ' ' || *token == '\t') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';
            
            if (strlen(token) > 0) {
                if_block->else_commands[if_block->else_count++] = strdup(token);
            }
            token = strtok(NULL, ";");
        }
    }
    
    free(copy);
    return 0;
}

// Execute an if-then-else block
int execute_if_block(if_block_t* if_block) {
    if (if_block == NULL || if_block->condition == NULL) {
        return -1;
    }
    
    printf("DEBUG: Condition: '%s'\n", if_block->condition);
    printf("DEBUG: Then commands: %d\n", if_block->then_count);
    for (int i = 0; i < if_block->then_count; i++) {
        printf("DEBUG: Then[%d]: '%s'\n", i, if_block->then_commands[i]);
    }
    printf("DEBUG: Else commands: %d\n", if_block->else_count);
    for (int i = 0; i < if_block->else_count; i++) {
        printf("DEBUG: Else[%d]: '%s'\n", i, if_block->else_commands[i]);
    }
    
    // Execute the condition command
    pipeline_t pipeline;
    int condition_result = -1;
    
    if (parse_redirection_pipes(if_block->condition, &pipeline) > 0) {
        if (pipeline.num_commands > 0) {
            condition_result = execute_single_command(&pipeline.commands[0]);
        }
        free_pipeline(&pipeline);
    } else {
        fprintf(stderr, "Error: failed to parse condition command: %s\n", if_block->condition);
        return -1;
    }
    
    printf("DEBUG: Condition result: %d\n", condition_result);
    
    // Execute appropriate block based on condition result
    if (condition_result == 0) {
        // Condition succeeded - execute then block
        printf("DEBUG: Executing THEN block\n");
        for (int i = 0; i < if_block->then_count; i++) {
            if (if_block->then_commands[i] != NULL) {
                printf("DEBUG: Executing then command: '%s'\n", if_block->then_commands[i]);
                if (parse_redirection_pipes(if_block->then_commands[i], &pipeline) > 0) {
                    execute_pipeline(&pipeline);
                    free_pipeline(&pipeline);
                } else {
                    fprintf(stderr, "Error: failed to parse then command: %s\n", if_block->then_commands[i]);
                }
            }
        }
    } else if (if_block->has_else) {
        // Condition failed and else exists - execute else block
        printf("DEBUG: Executing ELSE block\n");
        for (int i = 0; i < if_block->else_count; i++) {
            if (if_block->else_commands[i] != NULL) {
                printf("DEBUG: Executing else command: '%s'\n", if_block->else_commands[i]);
                if (parse_redirection_pipes(if_block->else_commands[i], &pipeline) > 0) {
                    execute_pipeline(&pipeline);
                    free_pipeline(&pipeline);
                } else {
                    fprintf(stderr, "Error: failed to parse else command: %s\n", if_block->else_commands[i]);
                }
            }
        }
    }
    
    return 0;
}

// Free memory allocated for if block
void free_if_block(if_block_t* if_block) {
    if (if_block == NULL) return;
    
    if (if_block->condition) {
        free(if_block->condition);
    }
    
    for (int i = 0; i < if_block->then_count; i++) {
        if (if_block->then_commands[i]) {
            free(if_block->then_commands[i]);
        }
    }
    
    for (int i = 0; i < if_block->else_count; i++) {
        if (if_block->else_commands[i]) {
            free(if_block->else_commands[i]);
        }
    }
}

// Check if a word is a control structure keyword
int is_control_keyword(const char* word) {
    return (strcmp(word, "if") == 0 ||
            strcmp(word, "then") == 0 ||
            strcmp(word, "else") == 0 ||
            strcmp(word, "fi") == 0);
}

// Simple multiline command reader
char* read_multiline_command(const char* initial_prompt) {
    return strdup(initial_prompt);
}

int parse_if_block_from_string(const char* full_command, if_block_t* if_block) {
    return parse_if_then_else(full_command, if_block);
}
