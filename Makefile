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
	rm ./owlscript
	rm *.o