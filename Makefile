main: 
	g++ --std=c++17 -c ./src/ast.cpp
	g++ --std=c++17 -c ./src/ast_builder.cpp
	g++ --std=c++17 -c ./src/interpreter/ast_interpreter.cpp
	g++ --std=c++17 -c ./src/interpreter/interpret_objects.cpp
	g++ --std=c++17 -c ./src/interpreter/interpret_statements.cpp
	g++ --std=c++17 -c ./src/interpreter/interpret_expressions.cpp
	g++ --std=c++17 -c ./src/interpreter/interpret_list_expressions.cpp
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
	g++ --std=c++17 -c -pg ./src/ast.cpp
	g++ --std=c++17 -c -pg ./src/ast_builder.cpp
	g++ --std=c++17 -c -pg ./src/interpreter/ast_interpreter.cpp
	g++ --std=c++17 -c -pg ./src/interpreter/interpret_objects.cpp
	g++ --std=c++17 -c -pg ./src/interpreter/interpret_statements.cpp
	g++ --std=c++17 -c -pg ./src/interpreter/interpret_expressions.cpp
	g++ --std=c++17 -c -pg ./src/interpreter/interpret_list_expressions.cpp
	g++ --std=c++17 -c -pg ./src/lex.cpp
	g++ --std=c++17 -c -pg ./src/object.cpp
	g++ --std=c++17 -c -pg ./src/parser.cpp
	g++ --std=c++17 -c -pg ./src/repl.cpp
	g++ --std=c++17 -c -pg ./src/resolve.cpp
	g++ --std=c++17 -c -pg ./src/token.cpp
	g++ --std=c++17 -c -pg ./src/main.cpp
	g++ --std=c++17 -pg *.o -o owlscript -lreadline
	rm *.o

