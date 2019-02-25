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
int yydebug =1;       // debug activé
%}

%union
{
 int number;
 char* chaine;
 n_exp* n_exp;
 n_prog* n_prog;
 n_l_dec* n_l_dec;
 n_l_exp* n_l_exp;
 n_appel* n_appel;
 n_instr* n_instr;
 n_l_instr* n_l_instr;
 n_var* n_var;
 n_dec* n_dec;
}


%token POINT_VIRGULE
%token PARENTHESE_FERMANTE
%token PARENTHESE_OUVRANTE
%token ACCOLADE_FERMANTE
%token ACCOLADE_OUVRANTE
%token EGAL
%token PLUS
%token MOINS
%token FOIS
%token DIVISE
%token NON
%token CROCHET_FERMANT
%token CROCHET_OUVRANT
%token INFERIEUR
%token SI
%token TANTQUE
%token SINON
%token ALORS
%token ET
%token OU
%token ENTIER
%token RETOUR
%token LIRE
%token ECRIRE
%token VIRGULE
%token FAIRE
%token <chaine> IDENTIF
%token <number> NOMBRE

%type <n_exp> expression
%type <n_exp> expression2
%type <n_exp> e2
%type <n_exp> e3
%type <n_exp> e4
%type <n_exp> e5
%type <n_exp> e6
%type <n_exp> e7

%type <n_l_exp> listeExp
%type <n_l_exp> listeExpBis

%type <n_instr> instruction
%type <n_l_instr> listeInstruction

%type <n_var> var

%type <n_appel> appelFct

%type <n_instr> instrBloc
%type <n_instr> instrSi
%type <n_instr> instrAffect
%type <n_instr> instrTantQue
%type <n_instr> instrAppel
%type <n_instr> instrRetour
%type <n_instr> instrEcriture
%type <n_instr> instrVide

%type <n_l_dec> listDecVariable
%type <n_l_dec> listDecVar
%type <n_l_dec> listDecVarBis
%type <n_dec> decVar
//%type <n_dec> tailleOpt
%type <n_l_dec> listArg

%type <n_l_dec> listDecFct
%type <n_l_dec> decFct

%type <n_prog> programme


//TODO: compléter avec la liste des terminaux

%start programme
%%

programme : listDecVariable listDecFct {n=cree_n_prog($1,$2);} ;

//Grammaire des déclarations de variables
listDecVariable : listDecVar POINT_VIRGULE {$$=$1;}
		| {$$=NULL;} ;
listDecVar : decVar listDecVarBis {$$=cree_n_l_dec($1,$2);} ;
listDecVarBis : VIRGULE decVar listDecVarBis {$$=cree_n_l_dec($2,$3);}| {$$=NULL;} ;
decVar : ENTIER IDENTIF {$$=cree_n_dec_var($2);}
 		| ENTIER IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT {$$=cree_n_dec_tab($2,$4);};


//Grammaire des déclarations de fonctions
listDecFct : decFct listDecFct {$$=cree_n_l_dec($1,$2);} | decFct {$$=cree_n_l_dec($1,NULL);} ;
decFct : IDENTIF PARENTHESE_OUVRANTE listArg PARENTHESE_FERMANTE listDecVariable instrBloc {$$=cree_n_l_dec($3,$5);} ;
listArg :  listDecVar  {$$=cree_n_l_dec($1,NULL);} | {$$=NULL;};


//TODO: compléter avec les productions de la grammaire

expression : expression2 {$$=$1;} ;

expression2 : expression2 OU e2 {$$=cree_n_exp_op(ou,$1,$3);}
    |e2 {$$=$1;}
    ;

e2 : e2 ET e3 {$$=cree_n_exp_op(et,$1,$3);}
    | e3 {$$=$1;}
    ;

e3 : e3 EGAL e4 {$$=cree_n_exp_op(egal,$1,$3);}
    | e3 INFERIEUR e4 {$$=cree_n_exp_op(inferieur,$1,$3);}
    | e4 {$$=$1;}
    ;

e4 : e4 PLUS e5 {$$=cree_n_exp_op(plus,$1,$3);}
    | e4 MOINS e5 {$$=cree_n_exp_op(moins,$1,$3);}
    | e5 {$$=$1;}
    ;

e5 : e5 FOIS e6 {$$=cree_n_exp_op(fois,$1,$3);}
    | e5 DIVISE e6 {$$=cree_n_exp_op(divise,$1,$3);}
    | e6 {$$=$1;}
    ;

e6 : NON e6 {$$=cree_n_exp_op(non,$2,NULL);}
    | e7 {$$=$1;}
    ;

e7 : PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE {$$=$2;}
    | NOMBRE						{$$=cree_n_exp_entier($1);}
    | appelFct						{$$=cree_n_exp_appel($1);}
    | var						{$$=cree_n_exp_var($1);}
    | LIRE						{$$=cree_n_exp_lire();}
    ;

appelFct : IDENTIF PARENTHESE_OUVRANTE listeExp PARENTHESE_FERMANTE {$$=cree_n_exp_appel($3);} ;

listeExp : expression listeExpBis {$$=cree_n_l_expl($1,$2);}
    |			{$$=NULL;}
    ;

listeExpBis : VIRGULE expression listeExpBis {$$=cree_n_l_expl($2,$3);}
      | 		{$$=NULL;}
      ;

var : IDENTIF {$$=cree_n_dec_var($1);}
    | IDENTIF CROCHET_OUVRANT expression CROCHET_FERMANT {$$=cree_n_dec_var($3);}
    ;

instruction : instrAffect {$$=$1;}
    | instrBloc {$$=$1;}
    | instrSi {$$=$1;}
    | instrTantQue {$$=$1;}
    | instrAppel {$$=$1;}
    | instrRetour {$$=$1;}
    | instrEcriture {$$=$1;}
    | instrVide {$$=$1;}
    ;

listeInstruction : instruction listeInstruction {$$=cree_n_l_instr($1,$2);}
    |			{$$=NULL;}
    ;

instrBloc : ACCOLADE_OUVRANTE listeInstruction ACCOLADE_FERMANTE {$$=cree_n_instr_bloc($2);} ;

instrAffect : var EGAL expression POINT_VIRGULE {$$=cree_n_instr_affect($1,$3);} ;

instrSi : SI expression ALORS instrBloc {$$=cree_n_instr_si($2,$4,NULL);}
    | SI expression ALORS instrBloc SINON instrBloc {$$=cree_n_instr_si($2,$4,$6);}
    ;

instrTantQue : TANTQUE expression FAIRE ACCOLADE_OUVRANTE instrBloc ACCOLADE_FERMANTE {$$=cree_n_instr_tantque($2,$5);} ;

instrAppel : appelFct POINT_VIRGULE {$$=cree_n_instruction_appel($1);} ;

instrRetour : RETOUR expression POINT_VIRGULE {$$=cree_n_instruction_retour($2);} ;

instrEcriture : ECRIRE PARENTHESE_OUVRANTE var PARENTHESE_FERMANTE POINT_VIRGULE {$$=cree_n_instruction_ecrire($3);} ;

instrVide : POINT_VIRGULE {$$=cree_n_instruction_vide();} ;


%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
