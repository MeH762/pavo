# The Pavo Programming Language

Pavo is a simple interpreted programming language created as a learning project. 

## Features:
- arithmetic operations: +, -
- logical operations: and (&), or (|), not (!)
- relational operators: !=, ==, <, >
- variable assignment, reassignment and usage (all variables are global)
- if statements
- simple loops
- blocks

## Syntax:
```bash
#creating a variable:
let x := 5;

#reassignment
x = x + 1;

let a := x > 2; #similar to c, int 0 mean False and int 1 is True
if a {
   print 1;
}
if !a {
   print 0;
}

#logical operations:
let p := x == 3
let q := p & 1;
q = !q;

#loops:
let i := 0;
loop i < 9 {
   print i;
   i = i + 1;
}

#blocks:
let k := {
   let l := 12;
   return l + x;
};
```

## How to run:
compile the source code, and run this:
```bash
./pavo <filename>.pavo
```

## Feedback:
This is a freshman project, and it is nowhere near finished. If you have any suggestions, tips, or if you just want to help out, feel free to reach out!
