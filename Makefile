owlscript:
	g++ --std=c++17 src/main.cpp -o owlscript

install:
	mv ./owlscript /usr/local/bin