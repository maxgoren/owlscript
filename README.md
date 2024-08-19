 ## Owlscript
 Owlscript is a lexically scoped and dynamically typed interpreted procedural language.

 Some features of note:
  - native list type with many built in operations
  - fist class functions
  - closures
  - struct types
  - iteration with while and for loops, also supports recursive functions and data structures.


### Some code examples

calculate fibonacci numbers

         def fib(var n) {
            if (n < 2) {
               return 1;
            } else {
               return fib(n-1) + fib(n-2);
            }
         }

List operations, such as map can be used both on named, or anonymous lists.

Using map operator on an anonymous list to get their fibonacci numbers:

         let fibd := map([9,10,11,12,13], fib);



      
