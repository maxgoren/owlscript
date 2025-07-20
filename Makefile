owlscript:
	gcc -c src/re2dfa/src/re2ast/src/ast.c 		-o build/re_ast.o
	gcc -c src/re2dfa/src/re2ast/src/parser.c 	-o build/re_parser.o
	gcc -c src/re2dfa/src/re2ast/src/tokens.c  	-o build/re_tokens.o
	gcc -c src/re2dfa/src/intset.c 			-o build/re_intset.o
	gcc -c src/re2dfa/src/followpos.c  		-o build/re_followpos.o
	gcc -c src/re2dfa/src/dfastate.c  		-o build/re_dfastate.o
	gcc -c src/re2dfa/src/dfa.c  			-o build/re_dfa.o
	gcc -c src/re2dfa/src/transition.c  	-o build/re_trans.o
	gcc -c src/re2dfa/src/re_to_dfa.c  		-o build/re_to_dfa.o
	gcc -c src/re2dfa/lex/lex.c  			-o build/re_lex.o
	gcc -c src/re2dfa/lex/util.c			-o build/re_util.o
	gcc -c src/re2dfa/lex/match.c           -o build/re_match.o
	g++ --std=c++17 -c src/allocator.cpp  	-o build/allocator.o
	g++ --std=c++17 -c src/ast.cpp  		-o build/ast.o
	g++ --std=c++17 -c src/astbuilder.cpp   -o build/astbuilder.o
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
	gcc -g -c -DDEBUG src/re2dfa/src/re2ast/src/ast.c 		-o build/re_ast.o
	gcc -g -c -DDEBUG src/re2dfa/src/re2ast/src/parser.c 	-o build/re_parser.o
	gcc -g -c -DDEBUG src/re2dfa/src/re2ast/src/tokens.c  	-o build/re_tokens.o
	gcc -g -c -DDEBUG src/re2dfa/src/intset.c 			-o build/re_intset.o
	gcc -g -c -DDEBUG src/re2dfa/src/followpos.c  		-o build/re_followpos.o
	gcc -g -c -DDEBUG src/re2dfa/src/dfastate.c  		-o build/re_dfastate.o
	gcc -g -c -DDEBUG src/re2dfa/src/transition.c  	-o build/re_trans.o
	gcc -g -c -DDEBUG src/re2dfa/src/dfa.c  			-o build/re_dfa.o
	gcc -g -c -DDEBUG src/re2dfa/src/re_to_dfa.c  		-o build/re_to_dfa.o
	gcc -g -c -DDEBUG src/re2dfa/lex/lex.c  			-o build/re_lex.o
	gcc -g -c -DDEBUG src/re2dfa/lex/util.c				-o build/re_util.o
	gcc -g -c -DDEBUG src/re2dfa/lex/match.c           -o build/re_match.o
	g++ --std=c++17 -g -c src/allocator.cpp  	-o build/allocator.o
	g++ --std=c++17 -g -c src/ast.cpp  		-o build/ast.o
	g++ --std=c++17 -g -c src/astbuilder.cpp   -o build/astbuilder.o
	g++ --std=c++17 -g -c src/owlscript.cpp  	-o build/owl_script.o
	g++ --std=c++17 -g -c src/object.cpp  		-o build/object.o
	g++ --std=c++17 -g -c src/parser.cpp  		-o build/parser.o
	g++ --std=c++17 -g -c src/resolve.cpp 		-o build/resolve.o
	g++ --std=c++17 -g -c src/token.cpp  		-o build/token.o
	g++ --std=c++17 -g -c src/twvm/context.cpp -o build/context.o
	g++ --std=c++17 -g -c src/twvm/twvm.cpp  	-o build/twvm.o
	g++ --std=c++17 -g -c src/twvm/twvm_expr.cpp  -o build/twvm_expr.o
	g++ --std=c++17 -g -c src/twvm/twvm_stmt.cpp  -o build/twvm_stmt.o
	g++ --std=c++17 -g -c src/twvm/twvm_listexpr.cpp  -o build/twvm_listexpr.o
	g++ --std=c++17 -g -c src/main.cpp   		-o build/main.o
	g++ --std=c++17 -g build/*.o -o owlscript

install:
	cp ./owlscript /usr/local/bin

clean:
	rm build/*.o