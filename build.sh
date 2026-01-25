#!/bin/sh
g++ -g src/owlscript.cpp -o owlscript
sudo mv owlscript /usr/local/bin
sudo mkdir /usr/local/bin/glauxVM
sudo cp src/vm/stdlib.owl /usr/local/bin/glauxVM
