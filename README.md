# Customized-Shell-Terminal (Project 2)
Develop customized Shell terminal for Linux machine ( Command-line Interface - CLI)


## Equipment:

Linux machine

## Introduction:

Command line shells provide a command-line interface (CLI) to the operating system, while graphical shells provide a graphical user interface (GUI). Other possibilities, although not so common, include voice user interface and various implementations of a text-based user interface (TUI) that are not CLI.

The outermost layer of the operating system is called the shell. 

In Unix-based systems, the shell is generally a command line interface. Most Linux distributions ship with bash as the default 
(there are several others: csh, ksh, sh, tcsh, zsh). 

In this project, we’ll be implementing a customize shell


## Implemented Command

1) ./hello
2) ls /usr
3) echo text
4) cd, job, history
5) whoami
6) mkdir
7) pwd
8) !!
9) !num
10) !prefix

Our shell supports scripting and interactive command line mode.

When implementing scripting mode, we will close stdin on the child process if call to exec() fails. This prevents infinite loops. 

## Signal Handling

1) Ctrl+C
2) Sig-child handler


## Miscellaneous:

Our shell supports the following:

* Background Jobs
* Incremental History Search
* Tab Completion


## Detail Implementation

In our design, we used <readline/readline.h> library to read the command and prefix. 
The project sub-models are listed here: 

```bash
history .c .h
ui .c .h
util .c .h
Makefile
user .c .h
parser .c .h
shell .c
```

## Build
The project can be built using the following command:

```bash
make
```

## Run
The project can be run using the following command:

```bash
./crash
```

## Test
The project can be run using the following command:

```bash
make test
make testupdate
```

Ref: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
