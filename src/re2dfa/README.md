 # re2dfa
Constructs a DFA from a regular expression using
the direct method outlined in the red dragon book.

The algorithm skips the need to create a separate NFA to perform subset construction on,
instead outputting a DFA directly by constructing firstpos, lastpos, and followpos
sets for each position in the RE string by performing DFS traversals on
the augmented AST.

## Steps

1) A Regular Expression is Augmented with a special "END" marker - '#'.
    re   ->  (re)#

2) An AST is then built for the augmented re.

3) The nodes are numbered, and using the rules for nullable(node) we calculate the following sets:

nullable(node): 

      returns true for nodes which can represent the empty set (epsilon)

Firstpos:

      the set of positions that can be the first symbol in a string derived from that node.

Lastpos:

      the set of positions that can be the last symbol in a string derived from that node.

Followpos:

      Defines how positions relate to each other during DFA state transitions. It determines which positions can follow a given position in a string derived from that node. 



4) From the Followpos table, we can build a DFA to recognize the provided regular expression.