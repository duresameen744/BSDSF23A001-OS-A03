#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

// Check if readline is available by testing its existence
#if __has_include(<readline/readline.h>) && __has_include(<readline/history.h>)
#include <readline/readline.h>
#include <readline/history.h>
#define USE_READLINE 1
#else
// Fallback: define dummy functions if readline not available
static inline char* readline(const char* prompt) {
    printf("%s", prompt);
    char* line = malloc(512);
    if (fgets(line, 512, stdin)) {
        // Remove newline
        char* newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        return line;
    }
    free(line);
    return NULL;
}

static inline void add_history(const char* line) {
    // Do nothing - fallback
    (void)line;
}

static inline char** rl_completion_matches(const char* text, char*(*generator)(const char*, int)) {
    (void)text;
    (void)generator;
    return NULL;
}

static inline char* rl_filename_completion_function(const char* text, int state) {
    (void)text;
    (void)state;
    return NULL;
}

// Dummy variables for readline
char* rl_readline_name = "";
int rl_completion_query_items = 100;
rl_completion_func_t* rl_attempted_completion_function = NULL;
#endif

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "FCIT> "
#define HISTORY_SIZE 20
#define MAX_PIPES 10  // NEW: Maximum number of pipes in a command

// NEW: Structure to hold command information with redirection
typedef struct {
    char* args[MAXARGS];     // Command arguments
    char* input_file;        // File for input redirection (<)
    char* output_file;       // File for output redirection (>)
    int background;          // Run in background (&) - for next feature
} command_t;

// NEW: Structure to hold pipeline information
typedef struct {
    command_t commands[MAX_PIPES];  // Commands in the pipeline
    int num_commands;               // Number of commands in pipeline
} pipeline_t;

// Function prototypes
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char** arglist);
int handle_builtin(char** arglist);

// History function prototypes
void add_to_history(const char* cmd);
void print_history();
char* get_history_command(int n);
int is_history_command(const char* cmdline);
char* expand_history_command(const char* cmdline);

// Readline-based command reader
char* read_cmd_readline(const char* prompt);
void initialize_readline();

// NEW: Redirection and pipe function prototypes
int parse_redirection_pipes(char* cmdline, pipeline_t* pipeline);
void free_pipeline(pipeline_t* pipeline);
int execute_redirection(command_t* cmd);
int execute_pipeline(pipeline_t* pipeline);
int execute_single_command(command_t* cmd);

#endif // SHELL_H
