/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED
# define YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    POINT_VIRGULE = 258,
    PARENTHESE_FERMANTE = 259,
    PARENTHESE_OUVRANTE = 260,
    ACCOLADE_FERMANTE = 261,
    ACCOLADE_OUVRANTE = 262,
    EGAL = 263,
    PLUS = 264,
    MOINS = 265,
    FOIS = 266,
    DIVISE = 267,
    NON = 268,
    CROCHET_FERMANT = 269,
    CROCHET_OUVRANT = 270,
    INFERIEUR = 271,
    SI = 272,
    TANTQUE = 273,
    POUR = 274,
    SINON = 275,
    ALORS = 276,
    ET = 277,
    OU = 278,
    ENTIER = 279,
    RETOUR = 280,
    LIRE = 281,
    ECRIRE = 282,
    VIRGULE = 283,
    FAIRE = 284,
    IDENTIF = 285,
    NOMBRE = 286
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 49 "analyseur_syntaxique.y" /* yacc.c:1909  */

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

#line 100 "analyseur_syntaxique.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED  */
