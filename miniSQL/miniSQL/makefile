LEX=lex
YACC=yacc
CC=g++
OBJECT=main

$(OBJECT): lex.yy.c  y.tab.c y.tab.h
	$(CC)   lex.yy.c    y.tab.c catalogmanager.cpp recordmanager.cpp buffermanager.cpp indexmanager.cpp base.cpp block.cpp api.cpp bplustree.cpp -std=c++11

lex.yy.c: test.l
	$(LEX) test.l

y.tab.c  y.tab.h: test.y
	$(YACC) -d test.y

clean:
	rm -f $(OBJECT)  *.o