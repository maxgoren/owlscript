owlscript:
	g++ -c src/parse/ast.cpp
	g++ -c src/parse/parser.cpp
	g++ -c src/owlscript.cpp
	g++ *.o -o owlscript

install:
	mv owlscript /usr/local/bin

clean:
	rm *.o
