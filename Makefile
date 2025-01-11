main: 
	g++ --std=c++17 -c ./src/ast.cpp
	g++ --std=c++17 -c ./src/ast_builder.cpp
	g++ --std=c++17 -c ./src/ast_interpreter.cpp
	g++ --std=c++17 -c ./src/lex.cpp
	g++ --std=c++17 -c ./src/object.cpp
	g++ --std=c++17 -c ./src/parser.cpp
	g++ --std=c++17 -c ./src/repl.cpp
	g++ --std=c++17 -c ./src/resolve.cpp
	g++ --std=c++17 -c ./src/token.cpp
	g++ --std=c++17 -c ./src/main.cpp
	g++ --std=c++17  *.o -o owlscript -lreadline
	rm *.o

debug:
	g++ --std=c++17 -c -g ./src/ast.cpp
	g++ --std=c++17 -c -g ./src/ast_builder.cpp
	g++ --std=c++17 -c -g ./src/ast_interpreter.cpp
	g++ --std=c++17 -c -g ./src/lex.cpp
	g++ --std=c++17 -c -g ./src/object.cpp
	g++ --std=c++17 -c -g ./src/parser.cpp
	g++ --std=c++17 -c -g ./src/repl.cpp
	g++ --std=c++17 -c -g ./src/resolve.cpp
	g++ --std=c++17 -c -g ./src/token.cpp
	g++ --std=c++17 -c -g ./src/main.cpp
	g++ --std=c++17 -g *.o -o owlscript -lreadline
	rm *.o

