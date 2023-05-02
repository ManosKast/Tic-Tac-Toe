CXX   = g++
CXXFLAGS = -std=c++20 -g -Wall -pedantic

EXE = main

SOBJ =  parser lexer


.PHONY: all clean

all: wc
	./main test1.txt

wc: symbol_table.cc
	$(MAKE) $(SOBJ)
	$(MAKE) symbol_table.o
	$(CXX) $(CXXFLAGS) -o $(EXE) symbol_table.o parser.o lexer.o $(LIBS)


parser: parser_clone.yy
	bison -d -v -o parser.tab.cc parser_clone.yy
	$(CXX) $(CXXFLAGS) -c -o parser.o parser.tab.cc

lexer: al.l
	flex++  $<
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o lexer.o


clean:
	rm -rf parser.tab.cc parser.tab.hh location.hh parser.output parser.o lexer.o lex.yy.cc symbol_table.o main