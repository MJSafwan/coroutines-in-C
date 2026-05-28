# Coroutines in C

Implementation for coroutines in ARM64 assembly for MacOS.

**NOTE: I implemented this as a learning resource and because it was fun to implement. This should NEVER be used in any serious project.**

# Example

There is a directory called "examples" that shows some example usecases of such library.
These showcase a simple demo in the "1-demo" directory, and a demonstration of a simple tcp echo server in the "2-tcp\_echo" directory.


In the tcp echo server example, the port 8080 is used on address 127.0.0.1. This, however, can be modified in the source file by modifiying the HOST\_PORT and HOST\_ADDR macros. 

# Building

To build the examples, you must have clang or compatible compiler installed.
After cloning, go to the appropriate example directory and run
```sh
make
```
A file named "main" should be produced.

# License

This project is licensed under the MIT License.
