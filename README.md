# bytevm compiler

The main purpose of this project is a stepping point towards porting owlscript
(http://github.com/maxgoren/owlscript) to a bytecode vm instead of its current 
tree walking interpreter. My other option was using the p-code vm from my
dalgol compiler, but Seeing as the p-machine is literally 1970s technology, 
trying to shoehorn owlscript into _that_ vm, while simultaneously ignoring 50+ 
years of advancements in  VM implementation just seemed like a wasted effort.

And so here we are.
