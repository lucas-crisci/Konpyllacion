%{
#include<stdlib.h>
#include<stdio.h>
#define YYDEBUG 1
#include"syntabs.h" // pour syntaxe abstraite
extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
int yydebug=1;
%}

%union { int nval;
	char* idval;
	n_l_instr* nlinstr;
	n_instr* ninstr;
	n_exp* nexp;
	n_l_exp* nlexp;
	n_var* nvar;
	n_l_dec* nldec;
	n_dec* ndec;
	n_prog* nprog;
	n_appel* nappel;
}

%token POINT_VIRGULE
%token PLUS
%token MOINS
%token FOIS
%token DIVISE
%token PARENTHESE_OUVRANTE
%token PARENTHESE_FERMANTE
%token CROCHET_OUVRANT
%token CROCHET_FERMANT
%token ACCOLADE_OUVRANTE
%token ACCOLADE_FERMANTE
%token EGAL
%token INFERIEUR
%token ET
%token OU
%token NON
%token SI
%token ALORS
%token SINON
%token TANTQUE
%token FAIRE
%token ENTIER
%token RETOUR
%token LIRE
%token ECRIRE
%token <idval>IDENTIF
%token <nval> NOMBRE
%token VIRGULE


%type <nprog> programme
%type <nlinstr> liste_instr
%type <ninstr> instr instrAffect instrSi instrTantque instrAppel instrRetour instrEcriture instrVide instrBloc
%type <nexp> expression expression2 expression3 expression4 expression5 expression6 expression7
%type <nlexp> liste_expression liste_expressionbis
%type <nvar> var
%type <nldec> listDeclarationVariable listArg listDeclarationVarBis listDeclarationVar listDeclarationFct
%type <ndec> declarationVar declarationFct

%type <nappel> appelfct

%start programme
%%


programme : listDeclarationVariable listDeclarationFct {n=cree_n_prog($1,$2);} ;

//Grammaire des déclarations de variables
listDeclarationVariable : {$$ = NULL;}
	| listDeclarationVar POINT_VIRGULE {$$ = $1;};
listDeclarationVar : declarationVar listDeclarationVarBis {$$ = cree_n_l_dec($1, $2);} ;
listDeclarationVarBis : VIRGULE declarationVar listDeclarationVarBis {$$ = cree_n_l_dec($2, $3);}
	| {$$ = NULL;} ;
declarationVar : ENTIER IDENTIF {$$ = cree_n_dec_var($2);}
	| ENTIER IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT {$$ = cree_n_dec_tab($2,$4);} ;

//Grammaire des déclarations de fonctions
listDeclarationFct : declarationFct listDeclarationFct {$$ = cree_n_l_dec($1, $2);}
	| declarationFct {$$ = cree_n_l_dec($1, NULL);} ;
declarationFct : IDENTIF PARENTHESE_OUVRANTE listArg PARENTHESE_FERMANTE listDeclarationVariable instrBloc {$$ = cree_n_dec_fonc($1, $3, $5, $6);};
listArg : {$$ = NULL;}
	| listDeclarationVar {$$ = $1;} ;


// Grammaire des expressions arithmétiques
expression: expression OU expression2 {$$ = cree_n_exp_op(ou,$1,$3);}
	| expression2 {$$ = $1;};
expression2: expression2 ET expression3 {$$ = cree_n_exp_op(et,$1,$3);}
	| expression3 {$$ = $1;};
expression3: expression3 EGAL expression4 {$$ = cree_n_exp_op(egal,$1,$3);}
	| expression3 INFERIEUR expression4 {$$ = cree_n_exp_op(inferieur,$1,$3);}
	| expression4 {$$ = $1;};
expression4: expression4 PLUS expression5 {$$ = cree_n_exp_op(plus,$1,$3);}
	| expression4 MOINS expression5 {$$ = cree_n_exp_op(moins,$1,$3);}
	| expression5 {$$ = $1;};
expression5: expression5 FOIS expression6 {$$ = cree_n_exp_op(fois,$1,$3);}
	| expression5 DIVISE expression6 {$$ = cree_n_exp_op(divise,$1,$3);}
	| expression6 {$$ = $1;};
expression6: NON expression6 {$$ = cree_n_exp_op(non, $2, NULL);}
	| expression7 {$$ = $1;};
expression7: PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE {$$ = $2;}
	| NOMBRE {$$ = cree_n_exp_entier($1);}
	| appelfct {$$ = cree_n_exp_appel($1);}
	| var {$$ = cree_n_exp_var($1);}
	| LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE{$$ = cree_n_exp_lire();};
var: IDENTIF {$$ = cree_n_var_simple($1);}
	| IDENTIF CROCHET_OUVRANT expression CROCHET_FERMANT{$$ = cree_n_var_indicee($1, $3);};
appelfct: IDENTIF PARENTHESE_OUVRANTE liste_expression PARENTHESE_FERMANTE {$$ = cree_n_appel($1, $3);};

liste_expression: expression liste_expressionbis {$$ = cree_n_l_exp($1, $2);}
	| {$$ = NULL;};
liste_expressionbis: VIRGULE expression liste_expressionbis {$$ = cree_n_l_exp($2, $3);}
	| {$$ = NULL;};


// Grammaire des instructions
instr: instrAffect {$$ = $1;}
	| instrBloc {$$ = $1;}
	| instrSi {$$ = $1;}
	| instrTantque{$$ = $1;}
	| instrAppel {$$ = $1;}
	| instrRetour {$$ = $1;}
	| instrEcriture {$$ = $1;}
	| instrVide {$$ = $1;} ;
liste_instr: instr liste_instr {$$ = cree_n_l_instr($1,$2);}
	| {$$ = NULL;};


instrAffect: var EGAL expression POINT_VIRGULE {$$ = cree_n_instr_affect($1,$3);};
instrBloc: ACCOLADE_OUVRANTE liste_instr ACCOLADE_FERMANTE {$$ = cree_n_instr_bloc($2);};
instrSi: SI expression ALORS instrBloc {$$ = cree_n_instr_si($2, $4, NULL);}
	| SI expression ALORS instrBloc SINON instrBloc {$$ = cree_n_instr_si($2, $4, $6);};

instrTantque: TANTQUE expression FAIRE instrBloc {$$ = cree_n_instr_tantque($2,$4);};
instrAppel: appelfct POINT_VIRGULE {$$ = cree_n_instr_appel($1);};
instrRetour: RETOUR expression POINT_VIRGULE {$$ = cree_n_instr_retour($2);};
instrEcriture: ECRIRE PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE POINT_VIRGULE {$$ = cree_n_instr_ecrire($3);};
instrVide: POINT_VIRGULE {$$ = cree_n_instr_vide();};

%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
