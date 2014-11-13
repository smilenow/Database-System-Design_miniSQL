LEX=lex
YACC=yacc
CC=g++
OBJECT=/Users/SmilENow/Desktop/runner

$(OBJECT): lex.yy.c  y.tab.c y.tab.h
	$(CC)   lex.yy.c    y.tab.c catalogmanager.cpp recordmanager.cpp buffermanager.cpp indexmanager.cpp base.cpp block.cpp api.cpp bplustree.cpp -o runner -std=c++11

lex.yy.c: test.lpp
	$(LEX) test.lpp

y.tab.c  y.tab.h: test.ypp
	$(YACC) -d test.ypp

clean:
	rm -f $(OBJECT)  *.o