%{
#include<stdlib.h>
#include<stdio.h>
#include "syntabs.h"
#include "affiche_arbre_abstrait.h"
#define YYDEBUG 1
//#include"syntabs.h" // pour syntaxe abstraite
extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
//int yydebug =1;       // debug activé
%}


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
%token NOMBRE
%token TANTQUE
%token SINON
%token ALORS
%token ET
%token OU
%token ENTIER
%token RETOUR
%token LIRE
%token ECRIRE
%token IDENTIF
%token VIRGULE
%token FAIRE

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

%type <n_var> variable
%type <n_appel> appelFct

%type <n_instr> instrBloc
%type <n_instr> instrSi
%type <n_instr> instrTantQue
%type <n_instr> instrAppel
%type <n_instr> instrRetour
%type <n_instr> instrEcriture
%type <n_instr> instrVide

%type <n_l_dec> listDecVariable
%type <n_l_dec> listDecVar
%type <n_l_dec> listDecVarBis
%type <n_dec> decVar

%type <n_l_dec> listDecFct
%type <n_l_dec> decFct


//TODO: compléter avec la liste des terminaux

%start programme
%%

programme : listDecVariable listDecFct {$$=cree_n_prog($1,$2); affiche_n_prog($$);} ;

//Grammaire des déclarations de variables
listDecVariable : | listDecVar POINT_VIRGULE ;
listDecVar : decVar listDecVarBis ;
listDecVarBis : VIRGULE decVar listDecVarBis | ;
decVar : type IDENTIF tailleOpt ;
tailleOpt : | taille ;
taille : CROCHET_OUVRANT expression CROCHET_FERMANT ;
type : ENTIER ;

//Grammaire des déclarations de fonctions
listDecFct : decFct listDecFct | decFct ;
decFct : IDENTIF PARENTHESE_OUVRANTE listArg PARENTHESE_FERMANTE listDecVariable instrBloc ;
listArg : | listDecVar ;


//TODO: compléter avec les productions de la grammaire

expression : expression2 {$$=$1;} ; 

expression2 : expression2 OU e2 {$$=cree_n_exp_op($2,$1,$3);}
    |e2 {$$=$1;}
    ;

e2 : e2 ET e3 {$$=cree_n_exp_op($2,$1,$3);}
    | e3 {$$=$1;}
    ;

e3 : e3 EGAL e4 {$$=cree_n_exp_op($2,$1,$3);}
    | e3 INFERIEUR e4 {$$=cree_n_exp_op($2,$1,$3);}
    | e4 {$$=$1;}
    ;

e4 : e4 PLUS e5 {$$=cree_n_exp_op($2,$1,$3);}
    | e4 MOINS e5 {$$=cree_n_exp_op($2,$1,$3);}
    | e5 {$$=$1;}
    ;

e5 : e5 FOIS e6 {$$=cree_n_exp_op($2,$1,$3);}
    | e5 DIVISE e6 {$$=cree_n_exp_op($2,$1,$3);}
    | e6 {$$=$1;}
    ;

e6 : NON e6 {$$=cree_n_exp_op($1,$2,NULL);}
    | e7 {$$=$1;}
    ;

e7 : PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE {$$=$2;}
    | NOMBRE						{$$=cree_n_exp_entier($1);}
    | appelFct						{$$=cree_n_exp_appel($1);}
    | var						{$$=cree_n_exp_var($1);}
    | LIRE						{$$=cree_n_exp_lire();}
    ;

appelFct : IDENTIF PARENTHESE_OUVRANTE listeExp PARENTHESE_FERMANTE;

listeExp : expression listeExpBis
    |			{$$=NULL;}
    ;

listeExpBis : VIRGULE expression listeExpBis
      | 		{$$=NULL;}
      ;

var : IDENTIF
    | IDENTIF CROCHET_OUVRANT expression CROCHET_FERMANT
    ;

instruction : instrAffect
    | instrBloc
    | instrSi
    | instrTantQue
    | instrAppel POINT_VIRGULE {$$=cree_n_instruction_appel($1);}
    | instrRetour
    | instrEcriture
    | instrVide
    ;

listeInstruction : instruction listeInstruction
    |			{$$=NULL;}
    ;

instrBloc : ACCOLADE_OUVRANTE listeInstruction ACCOLADE_FERMANTE;

instrAffect : var EGAL expression POINT_VIRGULE
    ;

instrSi : SI expression ALORS instrBloc
    | SI expression ALORS instrBloc SINON instrBloc
    ;

instrTantQue : TANTQUE expression FAIRE ACCOLADE_OUVRANTE instrBloc ACCOLADE_FERMANTE
    ;

instrAppel : instruction PARENTHESE_OUVRANTE listeExp PARENTHESE_FERMANTE
    | ECRIRE PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE
    |		{$$=NULL;}
    ;

instrRetour : RETOUR expression POINT_VIRGULE {$$=cree_n_instruction_retour($2);}
    ;

instrEcriture : ECRIRE PARENTHESE_OUVRANTE var PARENTHESE_FERMANTE POINT_VIRGULE {$$=cree_n_instruction_ecrire($3);}
    ;

instrVide : POINT_VIRGULE {$$=cree_n_instruction_vide();}
    ;


%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
