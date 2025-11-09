#include "shell.h"

// Execute a single command with redirection
int execute_redirection(command_t* cmd) {
    if (cmd == NULL || cmd->args[0] == NULL) {
        return -1;
    }

    // Handle built-in commands separately (they don't fork)
    if (handle_builtin(cmd->args)) {
        return 0;
    }

    int stdin_backup = -1;
    int stdout_backup = -1;
    int input_fd = -1;
    int output_fd = -1;

    // Handle input redirection
    if (cmd->input_file != NULL) {
        stdin_backup = dup(STDIN_FILENO); // Backup original stdin
        input_fd = open(cmd->input_file, O_RDONLY);
        if (input_fd < 0) {
            perror("open input file");
            if (stdin_backup >= 0) close(stdin_backup);
            return -1;
        }
        if (dup2(input_fd, STDIN_FILENO) < 0) {
            perror("dup2 input");
            close(input_fd);
            if (stdin_backup >= 0) close(stdin_backup);
            return -1;
        }
        close(input_fd);
    }

    // Handle output redirection
    if (cmd->output_file != NULL) {
        stdout_backup = dup(STDOUT_FILENO); // Backup original stdout
        output_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            perror("open output file");
            if (stdout_backup >= 0) close(stdout_backup);
            if (stdin_backup >= 0) {
                dup2(stdin_backup, STDIN_FILENO);
                close(stdin_backup);
            }
            return -1;
        }
        if (dup2(output_fd, STDOUT_FILENO) < 0) {
            perror("dup2 output");
            close(output_fd);
            if (stdout_backup >= 0) close(stdout_backup);
            if (stdin_backup >= 0) {
                dup2(stdin_backup, STDIN_FILENO);
                close(stdin_backup);
            }
            return -1;
        }
        close(output_fd);
    }

    // Execute the command
    int status;
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        waitpid(pid, &status, 0);
        
        // Restore original file descriptors
        if (stdin_backup >= 0) {
            dup2(stdin_backup, STDIN_FILENO);
            close(stdin_backup);
        }
        if (stdout_backup >= 0) {
            dup2(stdout_backup, STDOUT_FILENO);
            close(stdout_backup);
        }
        
        return WEXITSTATUS(status);
    } else {
        perror("fork");
        return -1;
    }
}

// Execute a pipeline of commands
int execute_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL || pipeline->num_commands == 0) {
        return -1;
    }

    // Single command (no pipes)
    if (pipeline->num_commands == 1) {
        return execute_single_command(&pipeline->commands[0]);
    }

    int num_commands = pipeline->num_commands;
    int pipefds[2 * (num_commands - 1)];
    pid_t pids[num_commands];
    int status = 0;

    // Create all pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            return -1;
        }
    }

    // Fork all commands
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // Child process
            
            // Set up input redirection
            if (i > 0) {
                // Read from previous pipe
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    exit(1);
                }
            } else if (pipeline->commands[i].input_file != NULL) {
                // Input file redirection for first command
                int input_fd = open(pipeline->commands[i].input_file, O_RDONLY);
                if (input_fd < 0) {
                    perror("open input file");
                    exit(1);
                }
                if (dup2(input_fd, STDIN_FILENO) < 0) {
                    perror("dup2 input file");
                    close(input_fd);
                    exit(1);
                }
                close(input_fd);
            }

            // Set up output redirection
            if (i < num_commands - 1) {
                // Write to next pipe
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    exit(1);
                }
            } else if (pipeline->commands[i].output_file != NULL) {
                // Output file redirection for last command
                int output_fd = open(pipeline->commands[i].output_file, 
                                   O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd < 0) {
                    perror("open output file");
                    exit(1);
                }
                if (dup2(output_fd, STDOUT_FILENO) < 0) {
                    perror("dup2 output file");
                    close(output_fd);
                    exit(1);
                }
                close(output_fd);
            }

            // Close all pipe file descriptors in child
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }

            // Execute the command
            if (pipeline->commands[i].args[0] != NULL) {
                // Check for built-in commands
                if (handle_builtin(pipeline->commands[i].args)) {
                    exit(0);
                }
                // Execute external command
                execvp(pipeline->commands[i].args[0], pipeline->commands[i].args);
                perror("execvp");
                exit(1);
            }
            exit(0);
        } else if (pids[i] < 0) {
            perror("fork");
            return -1;
        }
    }

    // Close all pipe file descriptors in parent
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return WEXITSTATUS(status);
}

// Execute a single command (with or without redirection)
int execute_single_command(command_t* cmd) {
    if (cmd == NULL || cmd->args[0] == NULL) {
        return -1;
    }

    // Check if there's any redirection
    if (cmd->input_file != NULL || cmd->output_file != NULL) {
        return execute_redirection(cmd);
    }

    // No redirection, use original execute function for built-in check
    if (handle_builtin(cmd->args)) {
        return 0;
    }

    // Execute external command without redirection
    return execute(cmd->args) == 0 ? 0 : -1;
}
