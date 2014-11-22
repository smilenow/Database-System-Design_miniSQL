LEX=lex
YACC=yacc
CC=g++
OBJECT=./runner

$(OBJECT): lex.yy.c  y.tab.c y.tab.h
	$(CC)   lex.yy.c    y.tab.c CatalogManager.cpp RecordManager.cpp BufferManager.cpp IndexManager.cpp Base.cpp Block.cpp API.cpp BPlusTree.cpp -o runner -std=c++11 -Wno-unused-value -Wno-deprecated-register -pthread

lex.yy.c: test.lpp
	$(LEX) test.lpp

y.tab.c  y.tab.h: test.ypp
	$(YACC) -d test.ypp

clean:
	rm -f $(OBJECT)  *.o
