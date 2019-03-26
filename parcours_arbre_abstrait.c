#include <stdio.h>
#include <stdlib.h>
#include "syntabs.h"
#include "util.h"
#include "tabsymboles.h"
#include "code3a.h"
#include <string.h>

void parcours_n_prog(n_prog *n);
void parcours_l_instr(n_l_instr *n);
void parcours_instr(n_instr *n);
void parcours_instr_si(n_instr *n);
void parcours_instr_tantque(n_instr *n);
void parcours_instr_affect(n_instr *n);
void parcours_instr_appel(n_instr *n);
void parcours_instr_retour(n_instr *n);
void parcours_instr_ecrire(n_instr *n);
void parcours_l_exp(n_l_exp *n);
operande* parcours_exp(n_exp *n);
operande* parcours_varExp(n_exp *n);
operande*  parcours_opExp(n_exp *n);
operande* parcours_intExp(n_exp *n);
operande* parcours_lireExp(n_exp *n);
void parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
operande* parcours_var(n_var *n);
void parcours_var_simple(n_var *n);
void parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);

int longueur_liste(n_l_dec *l_dec);
int longueur_args(n_l_exp *l_args);
char* new_e();

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;
int adresseGlobaleCourante = 0;
extern code3a_ code3a;
int cpt = 0;

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)     /*******************/
{
  code3a_init();

  printf("Start\n");

  portee = P_VARIABLE_GLOBALE;
  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);

  printf("End\n");

  int indice_fct;
  if((indice_fct = rechercheExecutable("main")) == -1) {
	  erreur("Main HS");
  }
  if(tabsymboles.tab[indice_fct].complement !=  0) {
		erreur("Le main doit avoir aucun args");
	}
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void parcours_l_instr(n_l_instr *n)
{
  if(n){
    parcours_instr(n->tete);
    parcours_l_instr(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr(n_instr *n)  /******************/
{
  if(n){
    if(n->type == blocInst) parcours_l_instr(n->u.liste);
    else if(n->type == affecteInst) parcours_instr_affect(n);
    else if(n->type == siInst) parcours_instr_si(n);
    else if(n->type == tantqueInst) parcours_instr_tantque(n);
    else if(n->type == appelInst) parcours_instr_appel(n);
    else if(n->type == retourInst) parcours_instr_retour(n);
    else if(n->type == ecrireInst) parcours_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_si(n_instr *n)
{
  operande *temporaire, *constante, *sinon, *fin;

  temporaire = parcours_exp(n->u.si_.test);

  constante = code3a_new_constante(0);

  sinon = code3a_new_etiquette(new_e());


  code3a_ajoute_instruction(jump_if_equal, temporaire, constante, sinon, NULL);

  parcours_instr(n->u.si_.alors);

  fin = code3a_new_etiquette(new_e());

  code3a_ajoute_instruction(jump, fin, NULL, NULL, NULL);

  if(n->u.si_.sinon){
    code3a_ajoute_etiquette(sinon->u.oper_nom);
    parcours_instr(n->u.si_.sinon);
  }
  code3a_ajoute_etiquette(fin->u.oper_nom);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{


	operande *test = code3a_new_etiquette(new_e());
	operande *suite = code3a_new_etiquette(new_e());
	code3a_ajoute_etiquette(test->u.oper_nom);
  operande *res = parcours_exp(n->u.tantque_.test);

	code3a_ajoute_instruction(jump_if_equal,res,code3a_new_constante(0),suite,NULL);
  parcours_instr(n->u.tantque_.faire);
	code3a_ajoute_instruction(jump,test,NULL,NULL,NULL);

	code3a_ajoute_etiquette(suite->u.oper_nom);


	/*operande* boucle;
 	boucle = code3a_new_etiquette(new_e());

 	code3a_ajoute_etiquette(boucle->u.oper_nom);

	 operande* temporaire = parcours_exp(n->u.tantque_.test);

	 operande* constante = code3a_new_constante(0);

	 operande* etiq_fin = code3a_new_etiquette(new_e());


	 code3a_ajoute_instruction(jump_if_equal, temporaire, constante, etiq_fin, "Instruction tant que");

	 parcours_instr(n->u.tantque_.faire);

	 code3a_ajoute_instruction(jump, boucle, NULL, NULL, "Retour au debut");

	code3a_ajoute_etiquette(etiq_fin->u.oper_nom);*/
}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)    /********************/
{
  operande *var, *exp;

  var = parcours_var(n->u.affecte_.var);
  exp = parcours_exp(n->u.affecte_.exp);

  code3a_ajoute_instruction(assign, exp, NULL, var, "Affectation");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)   /*********************/
{
  int indice_fct;
  if((indice_fct = rechercheExecutable(n->fonction)) == -1) {
    erreur("Une fonction doit être déclarée avant de l'appeler");
  }

  int nb_args = longueur_args(n->args);
  if(nb_args != tabsymboles.tab[indice_fct].complement) {
    erreur("Mauvais nombre d'arguments pour la fonction");
  }

  parcours_l_exp(n->args);

  char* nom = n->fonction;
  /*strcat(nom, n->fonction);*/
  operande* etiquette;
  etiquette = code3a_new_etiquette(nom);
  printf("9oper_type = %d\n", etiquette->oper_type);
  code3a_ajoute_instruction(func_call, etiquette, NULL, NULL, "Appel de fonction");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  operande* retour;
  retour = parcours_exp(n->u.retour_.expression);

  code3a_ajoute_instruction(func_val_ret, retour, NULL, NULL, "Instruction retour");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  operande* parametre;
  parametre = parcours_exp(n->u.ecrire_.expression);

  code3a_ajoute_instruction(sys_write, parametre, NULL, NULL, "Instruction ecrire");
}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{
  if(n){
    parcours_exp(n->tete);
    parcours_l_exp(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

operande* parcours_exp(n_exp *n)            /********************/
{
  operande* operande;

  if(n->type == varExp) operande = parcours_varExp(n);
  else if(n->type == opExp) operande = parcours_opExp(n);
  else if(n->type == intExp) operande = parcours_intExp(n);
  else if(n->type == appelExp) parcours_appelExp(n);
  else if(n->type == lireExp) operande = parcours_lireExp(n);

  return operande;
}

/*-------------------------------------------------------------------------*/

operande* parcours_varExp(n_exp *n)  /****************/
{
  int indice_var;
  if((indice_var = rechercheExecutable(n->u.var->nom)) == -1) {
    erreur("Une variable doit être déclarée avant utilisation");
  }

  if(n->u.var->type == simple){
    if(tabsymboles.tab[indice_var].type == T_TABLEAU_ENTIER)
      erreur("Une variable simple ne doit pas avoir d'indice");
  }
  else {
    if(tabsymboles.tab[indice_var].type == T_ENTIER)
      erreur("Un variable de type tableau doit avoir un indice");
  }

  operande *var;
  var = parcours_var(n->u.var);

  return var;
}

/*-------------------------------------------------------------------------*/
operande* parcours_opExp(n_exp *n)
{
  operation op = n->u.opExp_.op;
  operande *op1, *op2;
  instrcode code;

  if( n->u.opExp_.op1 != NULL ) {
    op1 = parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    op2 = parcours_exp(n->u.opExp_.op2);
  }

  switch (op)
  {
    case plus :
      code = arith_add;
      break;
    case moins:
      code = arith_sub;
      break;
    case fois:
      code = arith_mult;
      break;
    case divise:
      code = arith_div;
      break;
    case egal:
      code = jump_if_equal;
      break;
    case inferieur:
      code = jump_if_less;
      break;
    case ou:
      code = jump_if_equal ;
      break;
    case et:
      code = jump_if_equal;
      break;
    case non:
      code = jump_if_equal;
      break;
  }

  if(op < egal) {
    operande* temporaire;
    temporaire = code3a_new_temporaire();
    code3a_ajoute_instruction(code, op1, op2, temporaire, "Arithmétique / Logique");

    return temporaire;
  }

  operande* etiquette;
  etiquette = code3a_new_etiquette(new_e());

  code3a_ajoute_instruction(code, op1, op2, etiquette, "Saut test");

  operande *temporaire, *const_vrai, *const_faux, *test;
  temporaire = code3a_new_temporaire();

  const_vrai = code3a_new_constante(-1);

  const_faux = code3a_new_constante(0);

  test = code3a_new_etiquette(new_e());

  code3a_ajoute_instruction(assign, const_faux, NULL, temporaire, "Cas du test faux");
  code3a_ajoute_instruction(jump, test, NULL, NULL, NULL);
  code3a_ajoute_etiquette(etiquette->u.oper_nom);
  code3a_ajoute_instruction(assign, const_vrai, NULL, temporaire, "Cas du test vrai");
  code3a_ajoute_etiquette(test->u.oper_nom);

  return temporaire;
}

/*-------------------------------------------------------------------------*/

operande* parcours_intExp(n_exp *n)
{


  operande* constante;
  constante = code3a_new_constante(n->u.entier);


  return constante;
}

/*-------------------------------------------------------------------------*/
operande* parcours_lireExp(n_exp *n)
{
  operande* temporaire;
  temporaire = code3a_new_temporaire();

  code3a_ajoute_instruction(sys_read, NULL, NULL, temporaire, "Recuperation de la valeur de \"lire\"");

  return temporaire;
}

/*-------------------------------------------------------------------------*/

void parcours_appelExp(n_exp *n)
{
  parcours_appel(n->u.appel);
}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  if(n){

    parcours_dec(n->tete);

    parcours_l_dec(n->queue);


  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)      /*****************/
{
  if(n){
    if(n->type == foncDec) {
      parcours_foncDec(n);
    }
    else if(n->type == varDec) {
      parcours_varDec(n);
    }
    else if(n->type == tabDec) {
      parcours_tabDec(n);
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_foncDec(n_dec *n)  /*********************/
{
  int complement = longueur_liste(n->u.foncDec_.param);

  if(rechercheExecutable(n->nom) != -1) {
    erreur("Il y a déja une fonction qui porte ce nom");
  }



	char* nomtemp=malloc(102*sizeof(char));
	sprintf(nomtemp,"f%s",n->nom);

  operande* fonction = code3a_new_etiquette(nomtemp);

  code3a_ajoute_etiquette(fonction->u.oper_nom);
  code3a_ajoute_instruction(func_begin, NULL, NULL, NULL, "Début de la fonction");


  ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_FONCTION, 0, complement);
  entreeFonction();
  parcours_l_dec(n->u.foncDec_.param);
  portee = P_VARIABLE_LOCALE;
  parcours_l_dec(n->u.foncDec_.variables);

  parcours_instr(n->u.foncDec_.corps);


  code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin de la fonction");
  sortieFonction(1);
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)   /**********************/
{
  if(rechercheDeclarative(n->nom) == -1) {
    int complement = 1;
    int address;
    char* nom = n->nom;
    /*strcat(nom, n->nom);*/
    operande *var, *constante;

    switch(portee) {
      case P_VARIABLE_GLOBALE : address = adresseGlobaleCourante;
                                ajouteIdentificateur(n->nom, portee, T_ENTIER, address, complement);
                                var = code3a_new_var(nom, portee, address);

                                constante = code3a_new_constante(complement);

                                code3a_ajoute_instruction(alloc, constante, var, NULL, "Allocation de mémoire pour une Dec variable simple");
                                adresseGlobaleCourante += 4;
                                break;
      case P_VARIABLE_LOCALE : address = adresseLocaleCourante;
                               ajouteIdentificateur(n->nom, portee, T_ENTIER, address, complement);
                               var = code3a_new_var(nom, portee, address);

                               constante = code3a_new_constante(complement);

                               code3a_ajoute_instruction(alloc, constante, var, NULL, "Allocation de mémoire pour une Dec variable simple");
                               adresseLocaleCourante += 4;
                               break;
      case P_ARGUMENT : address = adresseArgumentCourant;
                                  ajouteIdentificateur(n->nom, portee, T_ENTIER, address, complement);

                                  adresseArgumentCourant += 4;
                                  break;
    }

  }
  else {
	  erreur("Variable simple déjà déclarée");
  }
}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)     /*******************/
{


  if(rechercheDeclarative(n->nom) == -1) {
    int address;
    int complement = n->u.tabDec_.taille;

    char* nom = "v";
    strcat(nom, n->nom);
    operande* var;

    address = adresseGlobaleCourante;
    ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, address, complement);
    var = code3a_new_var(nom, portee, address);

    adresseGlobaleCourante += 4*complement;

    operande* constante;
    constante = code3a_new_constante(complement);

    code3a_ajoute_instruction(alloc, constante, var, NULL, "Allocation de mémoire pour une Dec variable simple");
  }
  else
	erreur("Varaible tableau déjà déclarée");


}

/*-------------------------------------------------------------------------*/

operande* parcours_var(n_var *n)
{
  if(n->type == simple) {
    parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    parcours_var_indicee(n);
  }

  int indice_var = rechercheExecutable(n->nom);

  operande* var;
  var = code3a_new_var(n->nom, portee, tabsymboles.tab[indice_var].adresse);


  return var;
}

/*-------------------------------------------------------------------------*/
void parcours_var_simple(n_var *n)  /*********************/
{
  int indice_var;
  if((indice_var = rechercheExecutable(n->nom)) == -1) {
	  erreur("Une variable simple doit être déclarée");
  }

  if(tabsymboles.tab[indice_var].type == T_TABLEAU_ENTIER) {
	erreur("Une variable simple ne doit pas être utilisé avec un indice");
  }
}

/*-------------------------------------------------------------------------*/
void parcours_var_indicee(n_var *n)   /********************/
{
  int indice_var;
  if((indice_var = rechercheExecutable(n->nom)) == -1) {
	  erreur("Une variable tableau doit être déclarée avant utilisation");
  }

	if(tabsymboles.tab[indice_var].type == T_ENTIER) {
		erreur("Une variable tableau doit être utilisée avec un indice");
	}

  parcours_exp( n->u.indicee_.indice );
}

/*-------------------------------------------------------------------------*/
int longueur_liste(n_l_dec *l_dec)
{
  if(l_dec == NULL)
    return 0;

  return longueur_liste(l_dec->queue) + 1;
}

int longueur_args(n_l_exp *l_args) {
  if(l_args == NULL)
    return 0;

  return longueur_args(l_args->queue) + 1;
}

char* new_e() {
  int* num;
  num = &cpt;
  char* nom = malloc(sizeof(*nom) * 3);
  nom[0] = 'e';
  char c = cpt + '0';
  nom[1] = c;

  ++*num;

  return nom;
}
