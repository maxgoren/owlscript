owlscript:
	g++ --std=c++17 -c src/ast/ast.cpp
	g++ --std=c++17 -c src/ast/astbuilder.cpp
	g++ --std=c++17 -c src/lexer/lexer.cpp
	g++ --std=c++17 -c src/parser/parser.cpp
	g++ --std=c++17 -c src/buffer/stringbuffer.cpp
	g++ --std=c++17 -c src/object/object.cpp
	g++ --std=c++17 -c src/object/list.cpp
	g++ --std=c++17 -c src/object/gcalloc.cpp
	g++ --std=c++17 -c src/closure/closure.cpp
	g++ --std=c++17 -c src/memstore/memstore.cpp
	g++ --std=c++17 -c src/interpreter/itrace.cpp
	g++ --std=c++17 -c src/interpreter/listops.cpp
	g++ --std=c++17 -c src/interpreter/expressions.cpp
	g++ --std=c++17 -c src/interpreter/statements.cpp
	g++ --std=c++17 -c src/interpreter/callstack.cpp
	g++ --std=c++17 -c src/interpreter/interpreter.cpp
	g++ --std=c++17 -c src/interpreter/repl.cpp
	g++ --std=c++17 -c src/cli.cpp
	g++ *.o -o owlscript
	rm *.o

debug:
	g++ --std=c++17 -g -c src/ast/ast.cpp
	g++ --std=c++17 -g -c src/ast/astbuilder.cpp
	g++ --std=c++17 -g -c src/lexer/lexer.cpp
	g++ --std=c++17 -g -c src/parser/parser.cpp
	g++ --std=c++17 -g -c src/buffer/stringbuffer.cpp
	g++ --std=c++17 -g -c src/object/object.cpp
	g++ --std=c++17 -g -c src/object/list.cpp
	g++ --std=c++17 -g -c src/object/gcalloc.cpp
	g++ --std=c++17 -g -c src/interpreter/listops.cpp
	g++ --std=c++17 -g -c src/closure/closure.cpp
	g++ --std=c++17 -g -c src/memstore/memstore.cpp
	g++ --std=c++17 -g -c src/interpreter/itrace.cpp
	g++ --std=c++17 -g -c src/interpreter/expressions.cpp
	g++ --std=c++17 -g -c src/interpreter/statements.cpp
	g++ --std=c++17 -g -c src/interpreter/callstack.cpp
	g++ --std=c++17 -g -c src/interpreter/interpreter.cpp
	g++ --std=c++17 -g -c src/interpreter/repl.cpp
	g++ --std=c++17 -g -c src/cli.cpp
	g++ *.o -o owlscript
	mv *.o build/


install:
	cp owlscript /usr/local/bin
