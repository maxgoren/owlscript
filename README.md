# Owlscript

Owlscript is a general purpose multi-paradigm programming language
with a focus of blending concepts from functional programming and procedural programming, 
with ideas stemming from object oriented programming. 

## Notable Features
First class functions, lexical scoping, user defined structs, 
garbage collection, lambda functions, dynamic lists, regular expressionsand much more.

Running on the Glaux VM, programmers can write code interactively via the REPL
or run saved programs from disk. The `scripts` folder contains implementations 
of classic algorithms such as quicksort, mergesort, and red black trees.

## A Quick Tour

Variable assignment uses the let keyword, with variables being dynamically typed.
```
    let x := 13;
    let y := "hello there";
```

print outputs the variable to stdout, println does as well, with an appended new line character.
```
    print x;
    println y;
```

lists can be explicitly declared or generated from a range expression
```
    let xs := [1, 2, 3, 4, 5];
    let ys := [ 1 .. 5];
```

list elements can be accessed by index, 
```
    let idx := 0;
    while (idx < xs.size()) {
        println xs[idx];
        idx++;
    }
```

lists can also be iterated over using a foreach loop
```
    for (n of xs) {
        println t;
    }
```

lambdas composed of a single simple expression can be written with the arrow operator, otherwise a lambda can apply to a block of code.
```
    let ex := &(let m, let n) -> m+n;

    let yx := &(let m) { return m+n; };
```

lambdas can also be anonymous/executed in place.
```
    &(let msg) { let t := "hi " + msg; println t; return t;}("world");
```

procedures can be defined using either def or fn, both of which are syntactic sugar for lambda blocks.
```
    def fib(let k) {
        if (k < 2) {
            return 1;
        }
        return fib(k-1)+fib(k-2);
    }
```

user defined classes have _implicit_ constructors, which assign the result of supplied expressions in order of declaration
```
    class Node {
        let data;
        let left;
        let right;
    }

    let nn := new Node(42, nil, nil);
    nn.left := new Node(12, nil, nil);
```
