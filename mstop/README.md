# mstop
A simple CLI stopwatch written in c.

## Usage
- After building (see [readme](../README.md#usage))
```sh
./build/mstop
```
<!--- Help-->
<!--```sh-->
<!--./build/mstop --help-->
<!--```-->

## Screenshots
<img src="../img/mstop.png" width=500>

## References
- [hpc-tutorials.llnl.gov threads](https://hpc-tutorials.llnl.gov/posix/)
- [standard input without waiting for enter](https://www.tutorialpedia.org/blog/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed/)
- [Condition Variables for Signaling Between Threads](https://circuitlabs.net/pthreads-condition-variables-for-signaling-between-threads/)
    - checkout the image that explains when the mutex gets locked and unlocked when calling `pthread_cond_wait()`
