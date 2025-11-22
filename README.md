## MyShell — A Unix-Like Custom Command-Line Shell Implemented in C

MyShell is a custom Unix-style shell that supports interactive and batch execution, custom command processing, external program execution, input/output redirection, pipelines, conditional execution, and PATH-based lookup.  
It demonstrates low-level system programming with processes, file descriptors, and command parsing similar to real Unix shells.

## Features

### Command Execution
- Runs external programs (e.g., `ls`, `cat`, `grep`)
- Supports absolute and relative paths
- Resolves commands using the `PATH` environment variable

### Built-in Commands
MyShell includes a set of built-in commands implemented directly inside the shell:
- `cd <directory>` — change the current working directory  
- `pwd` — print the current working directory  
- `exit` — terminate the shell  

### Input and Output Redirection
- `command > file` — redirect standard output to a file  
- `command < file` — read standard input from a file  

### Pipelines
- `cmd1 | cmd2 | cmd3` — pipe commands so the output of one becomes the input of the next  

### Conditional Execution
- `cmd1 && cmd2` — run `cmd2` only if `cmd1` succeeds  
- `cmd1 || cmd2` — run `cmd2` only if `cmd1` fails  

### Modes
- **Interactive mode:** reads commands from the terminal  
- **Batch mode:** executes commands from a file  

## Repository Structure

```
/
├── src/            # Shell implementation (.c files)
├── include/        # Header files
├── tests/          # Test input files used in batch mode
├── Makefile        # Build script for MyShell
└── README.md       # Project documentation
```

## Building & Running

To compile the shell, run:

```
make
```

If you want to clean up object files and rebuild from scratch, run:

```
make clean
```

### Interactive Mode
Starts the shell and waits for user commands:

```
./mysh
```

You will see a prompt and can type commands directly.

### Batch Mode
Executes commands from a file instead of the terminal:

```
./mysh tests/piping.sh
```

In this mode, each line of the file is treated as a shell command and executed in order. Command output is printed to the terminal unless the command itself redirects output (e.g., using `>`).

## Example

The batch file (`tests/piping.sh`) demonstrates several features of MyShell, including file creation, input redirection, and pipelines:

```
ls
ls -la
echo "hello" > input.txt
echo hello > test1.txt
echo testing testing > pipetest.txt
cat < input.txt
cat < test1.txt
cat < pipetest.txt
ls
ls | sort
exit
```

Running:

```
./mysh tests/piping.sh
```

performs the following actions:

- Lists directory contents (`ls`, `ls -la`)
- Creates three text files using output redirection
- Uses input redirection (`cat < file`) to print each file to the terminal
- Runs a pipeline (`ls | sort`) to sort the directory listing
- Ends the session with `exit`

## Authors

Amy Margolina

Daniel Wang
