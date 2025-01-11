 ## Owlscript
 Owlscript is a lexically scoped and dynamically typed interpreted procedural language.

 Some features of note:
  - native list type with many built in operations
  - fist class functions
  - recursive functions / procedures / data structures.
  - closures
  - struct types
  - regular expression matching
  - list comprehensions with set builder notation
  - interactive development through the REPL or run owlscript programs from the command line

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

support for closures:

        def counter() {
            let a := 0;
            return &() { a := a + 1; return a; };
        }
        let c := counter();
        println c();

With owlscripts simple struct types implementing ADTs like binary search trees is a breeze:

       struct tnode {
          var key;
          var left;
          var right;
      }

      def insert(var tree, var v) {
          if (tree == nil) {
             tree := make tnode;
             tree[key] := v;
             tree[left] := nil;
             tree[right] := nil;
          } else {
             if (v < tree[key]) {
                tree[left] := insert(tree[left], v);
             } else {
               tree[right] := insert(tree[right], v);
             }
         }
         return tree;
      }

      def main() {
          let i := 0;
          let x := [ "owlscript", "is", "really", "cool!" ];
          let tree := nil;
          while (i < length(x)) {
              tree := insert(tree, x[i]);
              i := i + 1;
          }
      }
      
list comprehensions with set builder notation and owlscripts built-in range operator allow the quick production of lists

    Owlscript(0)> println [ 1 .. 100 | &(i) { i } | &(i) -> i % 5 == 0 ];
     [ 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100 ]
    Owlscript(1)> println [ 3 .. 13 | &(i) { i*i} ];
     [ 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169 ]
    Owlscript(2)>

Regular expression matching is available through the match() function

    Owlscript(0)> println match("aaaaabd", "(a*b|ac)d");
     true
    Owlscript(1)> let words := [ "rang", "rung", "ring", "ding"];
    Owlscript(2)> println map(words, &(x) -> match(x, "r[aeiou]n*."));
     [ true, true, true, false]
    Owlscript(3)>
