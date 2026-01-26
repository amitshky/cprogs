# mstop
A simple CLI stopwatch written in C. This project uses `pthread` so it will probably not work on Windows (maybe try [this](https://stackoverflow.com/questions/2150938/can-i-get-unixs-pthread-h-to-compile-in-windows)). This project also changes the behaviour of the terminal to take input, so by default, it will only work on Unix terminals (xterm, ...). Set the `--normal` option when running for it to work on other terminals, note that with this option, you will have to press `ENTER` after an input key press. Checkout `--help` for other options.

## Usage
- Build the project (see [readme](../README.md#usage)), then
```sh
./build/mstop
```
- Help
```sh
./build/mstop --help
```

### Keyboard inputs
|Key|Action|
|:--|:--|
|`s`|start/stop|
|`p` or `<space>`|pause/resume|
|`q`|quit|

## Screenshots
<img src="../img/mstop.png" width=500>

## References
- [hpc-tutorials.llnl.gov threads](https://hpc-tutorials.llnl.gov/posix/)
- [standard input without waiting for enter](https://www.tutorialpedia.org/blog/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed/)
- [Condition Variables for Signaling Between Threads](https://circuitlabs.net/pthreads-condition-variables-for-signaling-between-threads/)
    - checkout the image that explains when the mutex gets locked and unlocked when calling `pthread_cond_wait()`
