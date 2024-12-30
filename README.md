# README.md
# Pavo Programming Language

Pavo is a simple programming language implementation written in C. It supports basic arithmetic operations, variable assignments, control flow, and logical operations.

## Features

- Variable declarations and assignments
- Integer arithmetic
- Logical operations (AND, OR, NOT)
- Comparison operators
- If statements
- Print statements

## Building

To compile Pavo for your system:

```bash
# For standard compilation
gcc -o pavo src/pavo.c

# For universal binary (macOS)
gcc -arch x86_64 -arch arm64 -o pavo src/pavo.c
```

## Usage

Create a file with the `.pavo` extension:

```
let x := 42;
print x;

let y := x + 10;
print y;

if x < y {
    print 1;
}
```

Run it:

```bash
./pavo example.pavo
```

## Example Programs

Check the `examples/` directory for sample Pavo programs.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

# .gitignore
# Compiled files
pavo
*.o
*.out

# macOS specific
.DS_Store

# Editor specific
.vscode/
.idea/

# examples/hello.pavo
let message := 42;
print message;

let x := 10;
let y := 20;

if x < y {
    print 1;
}

# LICENSE
MIT License

Copyright (c) 2024 Aron J Dubovszky

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
