#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

#endif // SHELL_H
