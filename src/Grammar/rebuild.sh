#!/bin/sh
mgcpgen -lx owlscript.mgrm parser_tables.hpp
mv lexer_matrix.h ../lex
mv parser_tables.hpp ../parse
