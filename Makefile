LEX = flex
YACC = bison -d -t
CC = gcc

compilo: compilo.c analyseur_syntaxique.tab.o analyseur_lexical_flex.o
			$(CC) -o $@ $^

analyseur_syntaxique.tab.c : analyseur_syntaxique.y
			$(YACC) $<

analyseur_lexical_flex.c : analyseur_lexical.flex analyseur_syntaxique.tab.c
			$(FLEX) -o $@ $<

%.o: %.c
			$(CC) $(CCFLAGS) -c $^

clean:
	- rm -f $(OBJ)
	- rm -f compilo
	- rm -f test_yylex
	- rm -f analyseur_lexical_flex.c
