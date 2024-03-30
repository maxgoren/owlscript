# Owlscript

With Lexical scoping, dynamic typing, first class functions, recursion, lambdas, and built in lists,
you'd be forgiven for assuming OwlScript is yet another lisp, a peak "under the hood" will tell you
that it was actually _perl_ that was the greater influence on OwlScript. Now that half of you have
run for the hills, lets talk about what OwlScript can do!

Owlscript has a terse grammar, where variables are declared by use - no 'let' or 'var' keywords. 
'if' and 'loop' statements allow for easy control flow of Owlscript programs.

    def fib(m) {
        if (m < 2) {
            return 1;
        } 
        return fib(m-1) + fib(m-2);
    }
    i := 1;
    loop (i < 14) {
        print fib(i);
        i := i + 1;
    }

    
Lists are a built in data type in owlscript, with indexed access which mimics array like
behavior. the length of a list is returned through the built-in function 'length()'

    i := [13, 42, "yo dag", 6.66, 341];
    m := 0;
    loop (m < length(i)) {
	    print (i[m] + " ");
	    m := m + 1;
    }
    print "\n";

You can also print a list all at once instead of iterating through as above simply by using the print and println statement:

    heroes := ["batman", "superman", "snoop dogg", "emmanual west"];
    println heros;

Aside from indexed access, OwlScript also supports push/pop for use like a stack and append/pop for use as a queue.

    villains := [];
    push(villains, "joker");
    append(villains, "lex luther");
    push("sponge bob");
    #whoops
    pop(villains);

If your other car is a cdr, or you just prefer speaking with a bit of a lisp, OwlScript has some other familiar primitives as well:

    list := [13, 42, 86, 37, 24, 11];
    println list;
    i := 0;
    tail := list;
    loop (i < length(list)) {
        println i + ": " + first(tail);
        tail := rest(tail);
        i := i + 1;
    }
    print "\n";
    println list;
