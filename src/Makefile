main:
	g++ --std=c++17 -c ast.cpp
	g++ --std=c++17 -c ast_interpreter.cpp
	g++ --std=c++17 -c lex.cpp
	g++ --std=c++17 -c object.cpp
	g++ --std=c++17 -c parser.cpp
	g++ --std=c++17 -c token.cpp
	g++ --std=c++17 -c main.cpp
	g++ --std=c++17  *.o -o owlscript
	rm *.o

debug:
	g++ --std=c++17 -c -g ast.cpp
	g++ --std=c++17 -c -g ast_builder.cpp
	g++ --std=c++17 -c -g ast_interpreter.cpp
	g++ --std=c++17 -c -g lex.cpp
	g++ --std=c++17 -c -g object.cpp
	g++ --std=c++17 -c -g parser.cpp
	g++ --std=c++17 -c -g token.cpp
	g++ --std=c++17 -c -g main.cpp
	g++ --std=c++17 -g *.o -o owlscript
	rm *.o