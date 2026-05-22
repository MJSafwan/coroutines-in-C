# Coroutines in C

Implementation for coroutines in ARM64 assembly for MacOS.

**NOTE: I implemented this as a learning resource and because it was fun to implement. This should NEVER be used in any serious project.**

# Example

The 'example.c' file shows an example of how the coroutine functionality works. The file 'scheduler.c' implements a simple scheduler that handles yielding, asynchronous sleeping, and awaiting other routines.

# Building

To build the example, you must have clang or compatible compiler installed.
After cloning, run
```sh
make
```

Then, an executable file 'example' should be produced.

# License

This project is licensed under the MIT License.
