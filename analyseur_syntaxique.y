%{
#include<stdlib.h>
#include<stdio.h>
#include "syntabs.h"
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

//...
//TODO: compléter avec la liste des terminaux

%start programme
%%

programme : listDecVariable listDecFct {n=cree_n_prog($1,$2);} ;

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


expression : expression2;

expression2 : expression2 OU e2
    |e2
    ;

e2 : e2 ET e3
    | e3
    ;

e3 : e3 EGAL e4
    | e3 INFERIEUR e4
    | e4
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

e7 : PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE
    | NOMBRE
    | appelFct
    | var
    | LIRE
    ;

appelFct : IDENTIF PARENTHESE_OUVRANTE listeExp PARENTHESE_FERMANTE;

listeExp : expression listeExpBis
    |
    ;

listeExpBis : VIRGULE expression listeExpBis
      |
      ;

var : IDENTIF
    | IDENTIF CROCHET_OUVRANT expression CROCHET_FERMANT
    ;

instruction : instrAffect
    | instrBloc
    | instrSi
    | instrTantQue
    | instrAppel
    | instrRetour
    | instrEcriture
    | instrVide
    ;

listeInstruction : instruction listeInstruction
    |
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
    |
    ;

instrRetour : RETOUR expression POINT_VIRGULE

instrEcriture : ECRIRE PARENTHESE_OUVRANTE var PARENTHESE_FERMANTE POINT_VIRGULE
    ;

instrVide : POINT_VIRGULE
    ;


%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
