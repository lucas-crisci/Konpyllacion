CC = gcc
FLEX = flex

LIBS = -lm
CCFLAGS = -Wall -ggdb
YACC = bison -d -t

OBJ = analyseur_lexical_flex.o analyseur_syntaxique.tab.o util.o syntabs.o affiche_arbre_abstrait.o parcours_arbre_abstrait.o tabsymboles.o code3a.o c3a2nasm.o

all: compilo

compilo: compilo.c $(OBJ)
	$(CC) -o $@ $^

syntabs.o: syntabs.c
	$(CC) $(CCFLAGS) -c $^

tabsymboles.o: tabsymboles.c
		$(CC) $(CCFLAGS) -c $^

affiche_arbre_abstrait.o: affiche_arbre_abstrait.c
	$(CC) $(CCFLAGS) -c $^

parcours_arbre_abstrait.o: parcours_arbre_abstrait.c tabsymboles.h
		$(CC) $(CCFLAGS) -c $^

util.o: util.c
	$(CC) $(CCFLAGS) -c $^

analyseur_syntaxique.tab.c : analyseur_syntaxique.y syntabs.h
	$(YACC) $<

analyseur_lexical_flex.c: analyseur_lexical.flex analyseur_syntaxique.tab.c
	$(FLEX) -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $^

.PHONY : clean

clean:
	- rm -f $(OBJ)
	- rm -f compilo
	- rm -f test_yylex
	- rm -f analyseur_lexical_flex.c
	- rm -f analyseur_syntaxique.tab.*
	- rm -f *.o
