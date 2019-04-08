#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboles.h"
/*#include "analyseur_syntaxique_yacc.tab.h"*/
#include "util.h"
#include "analyseur_lexical_flex.h"

extern char *yytext;   // déclaré dans analyseur_lexical
extern int  yylineno;  // déclaré dans analyseur_lexical
int indent_xml = 0;
int indent_step = 1; // mettre à 0 pour ne pas indenter

/*******************************************************************************
 * Affiche le message d'alerte donné en paramètre, avec le numéro de ligne
 ******************************************************************************/
void warning(char *message) {
  fprintf (stderr, "WARNING ligne %d : ", yylineno);
  fprintf (stderr, "%s\n", message);
}

/*******************************************************************************
 * Affiche le message d'erreur donné en paramètre, avec le numéro de ligne, puis
 * quitte l'exécution du compilateur.
 ******************************************************************************/
void erreur(char *message) {
  fprintf (stderr, "ERREUR ligne %d : ", yylineno);
  fprintf (stderr, "%s\n", message);
  exit(1);
}

/*******************************************************************************
 * Affiche le message d'alerte donné en paramètre, avec le numéro de ligne.
 * Le message d'alerte peut contenir un %s variable, qui sera donné en
 * argument s
 ******************************************************************************/
void warning_1s(char *message, char *s) {
  fprintf (stderr, "WARNING ligne %d : ", yylineno);
  fprintf( stderr, message, s );
  fprintf( stderr, "\n" );
}

/*******************************************************************************
 * Affiche le message d'erreur donné en paramètre, avec le numéro de ligne.
 * Le message d'erreur peut contenir un %s variable, qui sera donné en
 * argument s, puis quitte l'exécution du compilateur.
 ******************************************************************************/
void erreur_1s(char *message, char *s) {
  fprintf( stderr, "Ligne %d : ", yylineno );
  fprintf( stderr, message, s );
  fprintf( stderr, "\n" );
  exit(1);
}

/*******************************************************************************
 * La valeur de yytext est volatile et ne peut pas être utilisée directement
 * pour remplir les noeuds feuille de l'arbre abstrait (nombres, identificateurs
 * de variables et fonctions) et dans la table des symboles. La fonction
 * ci-dessous permet de créer une copie d'une chaîne de caractères passée en
 * paramètre. La nouvelle chaîne est allouée, copiée et son adresse est renvoyée
 * en retour de la fonction.
 ******************************************************************************/
char *duplique_chaine(char *src) {
  char *dest = malloc(sizeof(char) * strlen(src));
  strcpy(dest, src);
  return dest;
}

/*******************************************************************************
 * Fonction interne d'indentation
 ******************************************************************************/
void indent() {
    int i;
    for( i = 0; i < indent_xml; i++ ) {
      printf( "  " );
    }
}

/*******************************************************************************
 * Affiche une balise XML ouvrante indentée nommée fct_ (dans votre compilateur,
 * vous pouvez utiliser __FUNCTION__ comme paramètre si votre fonction a été
 * nommée comme un non-terminal). Si la valeur de trace_xml est zéro, rien ne
 * sera affiché (permet de désactiver l'affichage de l'arbre syntaxique).
 ******************************************************************************/
void affiche_balise_ouvrante(const char *fct_, int trace_xml) {
  if( trace_xml ) {
    indent();
    indent_xml += indent_step ;
    fprintf (stdout, "<%s>\n", fct_);
  }
}

/*******************************************************************************
 * Affiche une balise XML fermante indentée nommée fct_ (dans votre compilateur,
 * vous pouvez utiliser __FUNCTION__ comme paramètre si votre fonction a été
 * nommée comme un non-terminal). Si la valeur de trace_xml est zéro, rien ne
 * sera affiché (permet de désactiver l'affichage de l'arbre syntaxique).
 ******************************************************************************/
void affiche_balise_fermante(const char *fct_, int trace_xml) {
  if(trace_xml) {
    indent_xml -= indent_step ;
    indent();
    fprintf (stdout, "</%s>\n", fct_);
  }
}

/*******************************************************************************
 * Affiche un texte et échappe les caractères spéciaux XML tels que < (&lt;),
 * > (&gt;) et & (&amp;). Si la valeur de trace_xml est zéro, rien ne sera
 * affiché (permet de désactiver l'affichage de l'arbre syntaxique).
 ******************************************************************************/

void affiche_texte( char *texte_, int trace_xml ) {
  int i = 0;
  while( texte_[ i ] != '\0' ) {
    if( texte_[ i ] == '<' ) {
      fprintf( stdout, "&lt;" );
    }
    else if( texte_[ i ] == '>' ) {
      fprintf( stdout, "&gt;" );
    }
    else if( texte_[ i ] == '&' ) {
      fprintf( stdout, "&amp;" );
    }
    else {
      putchar( texte_[i] );
    }
    i++;
  }
}

/*******************************************************************************
 * même chose que la fonction affiche_texte, mais ajoute des indentations et un retour chariot
 ******************************************************************************/
void affiche_xml_texte( char *texte_, int trace_xml ) {
  indent();
  affiche_texte(texte_, trace_xml);
  putchar('\n');
}

/*******************************************************************************
 * Affiche une élément XML fct_ indenté, c'est-à-dire, une paire de balises
 * ouvrante et fermante, avec, à leur intérieur, un texte_ (avec caractères
 * spéciaux echappés). Si la valeur de trace_xml est zéro, rien ne sera affiché
 * (permet de désactiver l'affichage de l'arbre syntaxique).
 ******************************************************************************/
void affiche_element(char *fct_, char *texte_, int trace_xml) {
  if(trace_xml) {
    indent();
    if(texte_){
      fprintf (stdout, "<%s>", fct_ );
      affiche_texte( texte_, trace_xml );
      fprintf (stdout, "</%s>\n", fct_ );
    }
    else{
      fprintf (stdout, "<%s/>\n", fct_ );
    }
  }
}

/*******************************************************************************
 * Affiche une élément XML correspondant à une unité lexicale de code uc. Pour
 * cela, utilise la fonction affiche_element ci-dessus, avec le nom de l'élément
 * comme balise et sa valeur comme texte. Si la valeur de trace_xml est zéro,
 * rien ne sera affiché (permet de désactiver l'affichage de l'arbre syntaxique)
 ******************************************************************************/
void affiche_feuille(int uc, int trace_xml) {
  char nom[100], valeur[100];
  nom_token( uc, nom, valeur );
  affiche_element( nom, valeur, trace_xml );
}


/*******************************************************************************
 * Fonction utile pour faire avancer la tête de lecture quand un symbole
 * terminal c dans une règle de production de la grammaire se retrouve sous la
 * tête de lecture uc. Attention, uc est un pointeur vers l'unité courante de
 * votre analyseur. Il peut être modifié avec un appel à yylex si la fonction
 * réussit la reconnaissance du symbole (c-à-d si *uc == c). Dans ce cas, la
 * fonction affichera aussi l'élément XML feuille correspondant au terminal
 * reconnu. En cas d'échec, affiche un message d'erreur informatif. Si la valeur
 * de trace_xml est zéro, rien ne sera affiché (permet de désactiver l'affichage
 * de l'arbre syntaxique)
 ******************************************************************************/
void consommer( int c, int *uc, int trace_xml ) {
  if( *uc == c ){
    affiche_feuille(*uc, trace_xml);
    *uc = yylex(); /* consommer le caractère */
  }
  else { /* Message d'erreur attendu/trouvé */
    char nomC[100], valeurC[100],
         nomUC[100], valeurUC[100], messageErreur[250];
    nom_token( c, nomC, valeurC );
    nom_token( *uc, nomUC, valeurUC );
    sprintf( messageErreur, "%s '%s' attendu, %s '%s' trouvé",
             nomC, valeurC, nomUC, valeurUC );
    erreur( messageErreur );
  }
}
