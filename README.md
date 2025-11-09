
# BSDSF23A001-OS-A03 - Custom UNIX Shell

A custom UNIX shell implementation for Operating Systems Assignment 03.

## Features Implemented

### Feature 1: Base Shell
- Basic command execution using fork-exec-wait cycle
- Modular code structure

### Feature 2: Built-in Commands
- `cd` - Change directory
- `exit` - Terminate shell
- `help` - Display help message
- `jobs` - Display background jobs

### Feature 3: Command History
- Store last 20 commands
- `history` command to display history
- `!n` to re-execute command number n
- `!!` to re-execute previous command

### Feature 4: Tab Completion with Readline
- GNU Readline integration
- Tab completion for commands and filenames
- History navigation with arrow keys
- Advanced line editing

### Feature 5: I/O Redirection and Pipes
- Input redirection (`<`)
- Output redirection (`>`)
- Pipes (`|`) between commands
- File descriptor management

### Feature 6: Command Chaining and Background Execution
- Command chaining with semicolons (`;`)
- Background execution with ampersand (`&`)
- Job control with `jobs` command
- Zombie process cleanup

### Feature 7: if-then-else-fi Control Structure
- Conditional command execution
- Multi-line if-then-else blocks
- Support for then and else branches

### Feature 8: Shell Variables
- Variable assignment: `VARNAME=value`
- Variable expansion: `$VARNAME`
- `set` command to display variables
- Environment variable integration

### Feature 9: Git Workflow
- Fork and pull request workflow
- Issue reporting and fixing
- Code review process

## Building

```bash
make
