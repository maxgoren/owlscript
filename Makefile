main:
	g++ --std=c++17 -c ./src/ast.cpp
	g++ --std=c++17 -c ./src/ast_builder.cpp
	g++ --std=c++17 -c ./src/ast_interpreter.cpp
	g++ --std=c++17 -c ./src/lex.cpp
	g++ --std=c++17 -c ./src/object.cpp
	g++ --std=c++17 -c ./src/parser.cpp
	g++ --std=c++17 -c ./src/repl.cpp
	g++ --std=c++17 -c ./src/token.cpp
	g++ --std=c++17 -c ./src/main.cpp
	g++ --std=c++17  *.o -o owlscript
	rm *.o

debug:
	g++ --std=c++17 -c -g ast.cpp
	g++ --std=c++17 -c -g ast_builder.cpp
	g++ --std=c++17 -c -g ast_interpreter.cpp
	g++ --std=c++17 -c -g lex.cpp
	g++ --std=c++17 -c -g object.cpp
	g++ --std=c++17 -c -g parser.cpp
	g++ --std=c++17 -c -g repl.cpp
	g++ --std=c++17 -c -g token.cpp
	g++ --std=c++17 -c -g main.cpp
	g++ --std=c++17 -g *.o -o owlscript
	rm *.o