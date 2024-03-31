# Owlscript

With Lexical scoping, dynamic typing, first class functions, recursion, lambdas, and built in lists,
you'd be forgiven for assuming OwlScript is yet another lisp, a a quick test drive will tell you
that it was actually _perl_ that was the greater influence on OwlScript. Now that half of you have
run for the hills, lets talk about what OwlScript can do!

Owlscript has a terse grammar, where variables are declared by use - no 'let' or 'var' keywords.
And despite the influence from perl, there are no sigils cluttering the code up either.
'if' and 'loop' statements allow for easy control flow of Owlscript programs, while recursive
proeceudres allow for the implementation of familiar algorithms.

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
behavior. The length of a list is returned through the built-in function 'length()'

    i := [13, 42, "yo dag", 6.66, 341];
    m := 0;
    loop (m < length(i)) {
	    print (i[m] + " ");
	    m := m + 1;
    }
    print "\n";
    max@MaxGorenLaptop:~/owlscript$ owlscript testcode/testListIndex.owl 
    13.000000 42.000000 yo dag 6.660000 341.000000

You can also print a list all at once instead of iterating through as above simply by using the print and println statement:

    heroes := ["batman","superman","snoop dogg","emmanual west"];
    println heros;
    max@MaxGorenLaptop:~/owlscript$ owlscript testcode/heros.owl 
     [ "batman", "superman", "snoop dogg", "emmanual west" ]
    
Aside from indexed access, OwlScript also supports push/pop for use like a stack and append/pop for use as a queue.

    villains := [];
    push(villains, "joker");
    push(villains, "lex luther");
    append("sponge bob");
    #whoops
    pop(villains);
    append(villains, "professor chaos");
    println villains;
    max@MaxGorenLaptop:~/owlscript$ owlscript testcode/villains.owl 
     [ "lex luther", "joker", "professor chaos" ]

If your other car is a cdr, or you just prefer speaking with a bit of a lisp, OwlScript has some other familiar primitives as well:

    list := [13, 42, 86, 37, 24, 11];
    println list;
    mutated := map(lambda(x) { return x*2;}, list);
    println mutated;
    println first(list);
    println rest(list);
    
    max@MaxGorenLaptop:~/owlscript$ owlscript testcode/testListPrimitives.owl 
     [ 13.000000, 42.000000, 86.000000, 37.000000, 24.000000, 11.000000 ]
     [ 26.000000, 84.000000, 172.000000, 74.000000, 48.000000, 22.000000 ]
     13.000000
     [ 42.000000, 86.000000, 37.000000, 24.000000, 11.000000 ]

Owlscript also has a REPL for interactive programming sessions

    max@MaxGorenLaptop:~/owlscript$ owlscript -r
    [OwlScript 0.1]
    Owlscript> def fib(x) { if (x < 2) { return 1; } return fib(x-1) + fib(x-2); }
        '-> def fib(x) { if (x < 2) { return 1; } return fib(x-1) + fib(x-2); }
    Owlscript> fibs := map(lambda(x) { return fib(x); }, [3,4,5,6,7]);
        '-> fibs := map(lambda(x) { return fib(x); }, [3,4,5,6,7]);
    Owlscript> println fibs;
        '-> println fibs;
    [ 3.000000, 5.000000, 8.000000, 13.000000, 21.000000 ]
    Owlscript>