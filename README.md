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

```
    let x := 13;
    let y := "hello there";
```

```
    print x;
    println y;
```

```
    let xs := [1, 2, 3, 4, 5];
    let ys := [ 1 .. 5];
```

```
    let idx := 0;
    while (idx < xs.size()) {
        println xs[idx];
        idx++;
    }
```

```
    for (n of xs) {
        println t;
    }
```

```
    let ex := &(let m, let n) -> m+n;

    let yx := &(let m) { return m+n; };

    &(let msg) { let t := "hi " + msg; println t; return t;}("world");
```

```
    def fib(let k) {
        if (k < 2) {
            return 1;
        }
        return fib(k-1)+fib(k-2);
    }
```
