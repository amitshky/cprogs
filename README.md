# cprogs
Simple C programs.

## Pre-requisites
> **NOTE**: The programs in this repo are primarily for linux, so the following programs are mostly only available on linux.

### Build and Configure
- [gcc](https://gcc.gnu.org/)
- [make](https://www.gnu.org/software/make/)
- [bear](https://github.com/rizsotto/Bear)

## Usage
Unless specified (check the readme in the program directory), this is the common way to build and run all of the programs in this repo.

### Build and Run
- By default, this will build and run the programs with debug symbols
```sh
cd [program_directory]
make
```
- To build and run in without debug symbols
```sh
cd [program_directory]
make release
make run
```

### Install
```sh
cd [program_directory]
make install
```

### Uninstall
```sh
cd [program_directory]
make uninstall
```

### Others
- Help
```sh
# NOTE: the program binary is in [program_directory]/build/
[program_binary] --help
```
- To delete all the build files
```sh
cd [program_directory]
make clean
```
- To clean the project directory
```sh
cd [program_directory]
make clean_proj
```

## Programs
| Name                | Description                  |
| :------------------ | :--------------------------- |
| [mtimer](./mtimer/) | A simple CLI countdown timer |
