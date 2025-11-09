#include "shell.h"

// Parse command line for redirection and pipes
int parse_redirection_pipes(char* cmdline, pipeline_t* pipeline) {
    if (cmdline == NULL || pipeline == NULL) {
        return -1;
    }

    // Initialize pipeline
    pipeline->num_commands = 0;
    for (int i = 0; i < MAX_PIPES; i++) {
        pipeline->commands[i].input_file = NULL;
        pipeline->commands[i].output_file = NULL;
        pipeline->commands[i].background = 0;
        for (int j = 0; j < MAXARGS; j++) {
            pipeline->commands[i].args[j] = NULL;
        }
    }

    char* tokens[MAX_LEN];
    int token_count = 0;
    char* token = strtok(cmdline, " \t\n");
    
    // First pass: tokenize the entire command line
    while (token != NULL && token_count < MAX_LEN - 1) {
        tokens[token_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    tokens[token_count] = NULL;

    if (token_count == 0) {
        return -1; // Empty command
    }

    int cmd_index = 0;
    int arg_index = 0;
    int i = 0;

    while (i < token_count) {
        // Check for pipe symbol
        if (strcmp(tokens[i], "|") == 0) {
            pipeline->commands[cmd_index].args[arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
            i++;
            continue;
        }
        
        // Check for input redirection
        else if (strcmp(tokens[i], "<") == 0) {
            if (i + 1 < token_count) {
                pipeline->commands[cmd_index].input_file = strdup(tokens[i + 1]);
                i += 2;
            } else {
                fprintf(stderr, "Syntax error: no file specified for input redirection\n");
                return -1;
            }
        }
        
        // Check for output redirection
        else if (strcmp(tokens[i], ">") == 0) {
            if (i + 1 < token_count) {
                pipeline->commands[cmd_index].output_file = strdup(tokens[i + 1]);
                i += 2;
            } else {
                fprintf(stderr, "Syntax error: no file specified for output redirection\n");
                return -1;
            }
        }
        
        // Check for background execution (for next feature)
        else if (strcmp(tokens[i], "&") == 0) {
            pipeline->commands[cmd_index].background = 1;
            i++;
        }
        
        // Regular argument
        else {
            pipeline->commands[cmd_index].args[arg_index++] = strdup(tokens[i++]);
        }

        // Check bounds
        if (cmd_index >= MAX_PIPES) {
            fprintf(stderr, "Error: too many pipes (max %d)\n", MAX_PIPES);
            return -1;
        }
        if (arg_index >= MAXARGS - 1) {
            fprintf(stderr, "Error: too many arguments (max %d)\n", MAXARGS);
            return -1;
        }
    }

    // Terminate the last command's argument list
    if (arg_index > 0) {
        pipeline->commands[cmd_index].args[arg_index] = NULL;
    }
    
    pipeline->num_commands = cmd_index + 1;
    return pipeline->num_commands;
}

// Free memory allocated for pipeline
void free_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL) return;
    
    for (int i = 0; i < pipeline->num_commands; i++) {
        command_t* cmd = &pipeline->commands[i];
        
        if (cmd->input_file) {
            free(cmd->input_file);
            cmd->input_file = NULL;
        }
        if (cmd->output_file) {
            free(cmd->output_file);
            cmd->output_file = NULL;
        }
        
        for (int j = 0; j < MAXARGS && cmd->args[j] != NULL; j++) {
            free(cmd->args[j]);
            cmd->args[j] = NULL;
        }
    }
    pipeline->num_commands = 0;
}
