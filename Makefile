owlscript:
	g++ --std=c++17 -c src/allocator.cpp
	g++ --std=c++17 -c src/ast.cpp
	g++ --std=c++17 -c src/astbuilder.cpp
	g++ --std=c++17 -c src/lexer.cpp
	g++ --std=c++17 -c src/owlscript.cpp
	g++ --std=c++17 -c src/regex/nfa.cpp
	g++ --std=c++17 -c src/regex/patternmatcher.cpp
	g++ --std=c++17 -c src/regex/re_parser.cpp
	g++ --std=c++17 -c src/object.cpp
	g++ --std=c++17 -c src/parser.cpp
	g++ --std=c++17 -c src/resolve.cpp
	g++ --std=c++17 -c src/token.cpp
	g++ --std=c++17 -c src/twvm/context.cpp
	g++ --std=c++17 -c src/twvm/twvm.cpp
	g++ --std=c++17 -c src/twvm/twvm_expr.cpp
	g++ --std=c++17 -c src/twvm/twvm_listexpr.cpp
	g++ --std=c++17 -c src/twvm/twvm_stmt.cpp
	g++ --std=c++17 -c src/main.cpp 
	g++ --std=c++17 *.o -o owlscript

with_c_lexer:
	gcc -c src/re2dfa/re2ast/src/ast.c 		-o build/re_ast.o
	gcc -c src/re2dfa/re2ast/src/parser.c 	-o build/re_parser.o
	gcc -c src/re2dfa/re2ast/src/tokens.c  	-o build/re_tokens.o
	gcc -c src/re2dfa/src/intset.c 			-o build/re_intset.o
	gcc -c src/re2dfa/src/followpos.c  		-o build/re_followpos.o
	gcc -c src/re2dfa/src/dfastate.c  		-o build/re_dfastate.o
	gcc -c src/re2dfa/src/dfa.c  			-o build/re_dfa.o
	gcc -c src/re2dfa/src/re_to_dfa.c  		-o build/re_to_dfa.o
	gcc -c src/re2dfa/ex/lex.c  			-o build/re_lex.o
	gcc -c src/re2dfa/ex/util.c				-o build/re_util.o
	g++ --std=c++17 -c src/allocator.cpp  	-o build/allocator.o
	g++ --std=c++17 -c src/ast.cpp  		-o build/ast.o
	g++ --std=c++17 -c src/astbuilder.cpp   -o build/astbuilder.o
	g++ --std=c++17 -c src/lexer.cpp  		-o build/lexer.o
	g++ --std=c++17 -c src/owlscript.cpp  	-o build/owl_script.o
	g++ --std=c++17 -c src/object.cpp  		-o build/object.o
	g++ --std=c++17 -c src/parser.cpp  		-o build/parser.o
	g++ --std=c++17 -c src/resolve.cpp 		-o build/resolve.o
	g++ --std=c++17 -c src/token.cpp  		-o build/token.o
	g++ --std=c++17 -c src/twvm/context.cpp -o build/context.o
	g++ --std=c++17 -c src/twvm/twvm.cpp  	-o build/twvm.o
	g++ --std=c++17 -c src/twvm/twvm_expr.cpp  -o build/twvm_expr.o
	g++ --std=c++17 -c src/twvm/twvm_stmt.cpp  -o build/twvm_stmt.o
	g++ --std=c++17 -c src/twvm/twvm_listexpr.cpp  -o build/twvm_listexpr.o
	g++ --std=c++17 -c src/main.cpp   		-o build/main.o
	g++ --std=c++17 build/*.o -o owlscript

debug:
	g++ --std=c++17 -c -g src/allocator.cpp
	g++ --std=c++17 -c -g src/ast.cpp
	g++ --std=c++17 -c -g src/astbuilder.cpp
	g++ --std=c++17 -c -g src/lexer.cpp
	g++ --std=c++17 -c -g src/owlscript.cpp
	g++ --std=c++17 -c -g src/regex/nfa.cpp
	g++ --std=c++17 -c -g src/regex/patternmatcher.cpp
	g++ --std=c++17 -c -g src/regex/re_parser.cpp
	g++ --std=c++17 -c -g src/object.cpp
	g++ --std=c++17 -c -g src/parser.cpp
	g++ --std=c++17 -c -g src/resolve.cpp
	g++ --std=c++17 -c -g src/token.cpp
	g++ --std=c++17 -c -g src/twvm/context.cpp
	g++ --std=c++17 -c -g src/twvm/twvm.cpp
	g++ --std=c++17 -c -g src/twvm/twvm_expr.cpp
	g++ --std=c++17 -c -g src/twvm/twvm_listexpr.cpp
	g++ --std=c++17 -c -g src/twvm/twvm_stmt.cpp
	g++ --std=c++17 -c -g src/main.cpp 
	g++ --std=c++17 -g *.o -o owlscript_debug_ver

install:
	cp ./owlscript /usr/local/bin

clean:
	rm build/*.o