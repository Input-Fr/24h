# 24h - The Handmade POSIX Shell in C

## Overview
**24h** is a custom-built POSIX shell written in C. The goal of this project is to create a shell from scratch that follow the POSIX standard specified by the SCL.

## Features
- Supports everyday built-in commands
- Implements piping (`|`)
- Supports input/output redirection (`>`, `<`, `>>`, `<<`)
- Works with standard UNIX utilities (`cd`, `ls`, `cat`, etc.)
- Fully compatible with POSIX guidelines

## Execution flow overview
![](https://github.com/Input-Fr/42sh/blob/main/ExecutionFlow42sh.png?raw=true)

## Prerequisites
Ensure you have the following installed:
- `gcc` (GNU Compiler Collection)
- A UNIX-based operating system

## Installation
```sh
$> git clone https://github.com/yourusername/24h.git
$> cd 24h
$> autoreconf -if
$> ./configure
$> make install
$> 24h
```

## Usage
Use **24h** just like any other shell:

### Running Commands
```sh
$> ls -la
$> pwd
$> echo "Hello, World!"
```

### Using Pipes
```sh
$> ls | grep "main"
```

### Redirecting Output
```sh
$> echo "Hello" > file.txt  # Write to file
$> echo "World" >> file.txt  # Append to file
$> cat < file.txt           # Read from file
```

### Navigating Directories
```sh
$> cd /path/to/directory
$> pwd
```



