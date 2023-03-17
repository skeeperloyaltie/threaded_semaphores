## N readers and 1 writer problem
- The problem statement describes the n readers and 1 writer problem, which requires multiple threads to access a shared counter variable, with multiple readers able to read simultaneously but only one writer able to write at any time.

## How to run 
- Compile the program first using the ```-pthread``` argument <br>

```gcc -pthread -o program programm_c.c``` <br>

- Then run the program asserting the readers and writers as arguments <br>

```./program 12 12```

