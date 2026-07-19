owlscript:
	g++ --std=c++17 -c src/parse/ast.cpp
	g++ --std=c++17 -c src/parse/lexer.cpp
	g++ --std=c++17 -c src/parse/parser.cpp
	g++ --std=c++17 -c src/parse/parse_expr.cpp
	g++ --std=c++17 -c src/parse/parse_stmt.cpp
	g++ --std=c++17 -c src/compile/bcgen.cpp
	g++ --std=c++17 -c src/compile/compile_expr.cpp
	g++ --std=c++17 -c src/compile/compile_stmt.cpp
	g++ --std=c++17 -c src/compile/compile_load_store.cpp
	g++ --std=c++17 -c src/compile/scopingst.cpp
	g++ --std=c++17 -c src/vm/callframe.cpp
	g++ --std=c++17 -c src/vm/closure.cpp
	g++ --std=c++17 -c src/vm/constpool.cpp
	g++ --std=c++17 -c src/vm/gc.cpp
	g++ --std=c++17 -c src/vm/heapitem.cpp
	g++ --std=c++17 -c src/vm/stackitem.cpp
	g++ --std=c++17 -c src/vm/vm.cpp
	g++ --std=c++17 -c src/vm/regex/regex.cpp
	g++ --std=c++17 -c src/owlscript.cpp
	g++ *.o -o owlscript

install:
	mv owlscript /usr/local/bin

clean:
	rm *.o
