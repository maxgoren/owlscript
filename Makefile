owlscript:
	g++ --std=c++17 -c src/allocator.cpp
	g++ --std=c++17 -c src/ast.cpp
	g++ --std=c++17 -c src/astbuilder.cpp
	g++ --std=c++17 -c src/context.cpp
	g++ --std=c++17 -c src/lexer.cpp
	g++ --std=c++17 -c src/regex/nfa.cpp
	g++ --std=c++17 -c src/regex/patternmatcher.cpp
	g++ --std=c++17 -c src/regex/re_parser.cpp
	g++ --std=c++17 -c src/object.cpp
	g++ --std=c++17 -c src/parser.cpp
	g++ --std=c++17 -c src/resolve.cpp
	g++ --std=c++17 -c src/token.cpp
	g++ --std=c++17 -c src/twvm.cpp
	g++ --std=c++17 -c src/main.cpp 
	g++ --std=c++17 *.o -o owlscript

install:
	cp ./owlscript /usr/local/bin

clean:
	rm ./owlscript
	rm *.o