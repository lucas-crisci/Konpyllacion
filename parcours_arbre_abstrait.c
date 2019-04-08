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
operande* parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
operande* parcours_var(n_var *n);
operande* parcours_var_simple(n_var *n);
operande* parcours_var_indicee(n_var *n);
operande* parcours_appel(n_appel *n);

int longueur_liste(n_l_dec *l_dec);
int longueur_args(n_l_exp *l_args);

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;
extern int AFFtabSymboles;

int adresseGlobaleCourante = 0;
extern code3a_ code3a;

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)     /*******************/
{
  code3a_init();

  portee = P_VARIABLE_GLOBALE;
  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);

  int indice_fct;
  if((indice_fct = rechercheExecutable("main")) == -1) {
	  erreur("Main inexistant");
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
  operande *temporaire, *constante, *sinon, *fin, *etiq;

  temporaire = parcours_exp(n->u.si_.test);
  constante = code3a_new_constante(0);
  etiq = code3a_new_etiquette_auto();
  sinon = code3a_new_etiquette(etiq->u.oper_nom);
  code3a_ajoute_instruction(jump_if_equal, temporaire, constante, sinon, "lil jump");


  parcours_instr(n->u.si_.alors);

  //etiq = code3a_new_etiquette_auto();       //// j'ai delete ca

  fin = code3a_new_etiquette(etiq->u.oper_nom);

  if(n->u.si_.sinon){

    etiq = code3a_new_etiquette_auto();           //// j'ai add ca
    fin = code3a_new_etiquette(etiq->u.oper_nom); //// et ca

    code3a_ajoute_instruction(jump, fin, NULL, NULL, "got");

    code3a_ajoute_etiquette(sinon->u.oper_nom);
    parcours_instr(n->u.si_.sinon);

  }

  code3a_ajoute_etiquette(fin->u.oper_nom);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  operande* boucle;
  operande *etiq = code3a_new_etiquette_auto();
  boucle = code3a_new_etiquette(etiq->u.oper_nom);
  code3a_ajoute_etiquette(boucle->u.oper_nom);

  operande* temporaire = parcours_exp(n->u.tantque_.test);
  operande* constante = code3a_new_constante(0);

  etiq = code3a_new_etiquette_auto();
  operande* etiq_fin = code3a_new_etiquette(etiq->u.oper_nom);

  code3a_ajoute_instruction(jump_if_equal, temporaire, constante, etiq_fin, "Instruction tant que");

  parcours_instr(n->u.tantque_.faire);

  code3a_ajoute_instruction(jump, boucle, NULL, NULL, "Retour au debut");

  code3a_ajoute_etiquette(etiq_fin->u.oper_nom);
}



/*-------------------------------------------------------------------------*/
void parcours_instr_pour(n_instr *n)
{
  operande* pour;
  operande *etiq = code3a_new_etiquette_auto();
  pour = code3a_new_etiquette(etiq->u.oper_nom);

  parcours_exp(n->u.pour_.init);

  operande* temporaire = parcours_exp(n->u.pour_.test);
  operande* constante = code3a_new_constante(0);

  etiq = code3a_new_etiquette_auto();
  operande* etiq_fin = code3a_new_etiquette(etiq->u.oper_nom);

  code3a_ajoute_instruction(jump_if_less,temporaire,constante,etiq_fin,NULL );
  parcours_instr(n->u.pour_.faire);

  parcours_exp(n->u.pour_.affect);

  code3a_ajoute_instruction(jump, pour, NULL, NULL,NULL);
  code3a_ajoute_etiquette(etiq_fin->u.oper_nom);
}


/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)    /********************/
{
  operande *var, *exp;

  var = parcours_var(n->u.affecte_.var);
  exp = parcours_exp(n->u.affecte_.exp);

  if(var->u.oper_var.oper_indice != NULL && var->u.oper_var.oper_indice->oper_type == O_VARIABLE) {
    operande* tmp1 = code3a_new_temporaire();
    code3a_ajoute_instruction(assign, var->u.oper_var.oper_indice, NULL, tmp1, NULL);
    var->u.oper_var.oper_indice = tmp1;
  }
  if(exp->oper_type == O_VARIABLE && exp->u.oper_var.oper_indice != NULL && exp->u.oper_var.oper_indice->oper_type == O_VARIABLE) {
    operande* tmp2 = code3a_new_temporaire();
    code3a_ajoute_instruction(assign, exp->u.oper_var.oper_indice, NULL, tmp2, NULL);
    exp->u.oper_var.oper_indice = tmp2;
  }

  code3a_ajoute_instruction(assign, exp, NULL, var, "Affect");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
  char nom[20];
  strcpy(nom, "f");
  strcat(nom, n->u.appel->fonction);

  code3a_ajoute_instruction(func_call, code3a_new_etiquette(nom), NULL, NULL, "appel func");
}
/*-------------------------------------------------------------------------*/

operande* parcours_appel(n_appel *n)   /*********************/
{
  int indice_fct;
  if((indice_fct = rechercheExecutable(n->fonction)) == -1) {
    erreur("Une fonction doit être déclarée avant de l'appeler");
  }

  operande* constante;
  constante = code3a_new_constante(1);
  code3a_ajoute_instruction(alloc, constante, NULL, NULL," alloue place pour la valeur de retour");


  int nb_args = longueur_args(n->args);               ///////////////////// +param
  if(nb_args != tabsymboles.tab[indice_fct].complement) {
    erreur("Mauvais nombre d'arguments pour la fonction");
  }
  int portee_bis = portee;
  portee = P_ARGUMENT;
  parcours_l_exp(n->args);
  portee = portee_bis;

  //char* nom = n->fonction;
  //operande* etiquette = code3a_new_etiquette(nom);
  operande* temporaire = code3a_new_temporaire();

  return temporaire;
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  operande* retour;
  retour = parcours_exp(n->u.retour_.expression);

  code3a_ajoute_instruction(func_val_ret, retour, NULL, NULL, "Instruction retour");
  code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin de la fonction");

}
/*-------------------------------------------------------------------------*/


void parcours_instr_ecrire(n_instr *n)
{
  operande* parametre;
  parametre = parcours_exp(n->u.ecrire_.expression);

  if(parametre->oper_type == O_VARIABLE && parametre->u.oper_var.oper_indice != NULL) {
    operande* temporaire = code3a_new_temporaire();
    code3a_ajoute_instruction(assign, parametre->u.oper_var.oper_indice, NULL, temporaire, NULL);
    parametre->u.oper_var.oper_indice = temporaire;
  }

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
  operande *operande;

  if(n->type == varExp) operande = parcours_varExp(n);
  else if(n->type == opExp) operande = parcours_opExp(n);
  else if(n->type == intExp) operande = parcours_intExp(n);
  else if(n->type == appelExp) operande = parcours_appelExp(n);
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

  if(op1->oper_type == O_VARIABLE && op1->u.oper_var.oper_indice != NULL && op1->u.oper_var.oper_indice->oper_type == O_VARIABLE) {
    operande* tmp1 = code3a_new_temporaire();
    code3a_ajoute_instruction(assign, op1->u.oper_var.oper_indice, NULL, tmp1, NULL);
    op1->u.oper_var.oper_indice = tmp1;
  }
  if(op2->oper_type == O_VARIABLE && op2->u.oper_var.oper_indice != NULL && op2->u.oper_var.oper_indice->oper_type == O_VARIABLE) {
    operande* tmp2 = code3a_new_temporaire();
    code3a_ajoute_instruction(assign, op2->u.oper_var.oper_indice, NULL, tmp2, NULL);
    op2->u.oper_var.oper_indice = tmp2;
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

    if(portee == P_ARGUMENT)
      code3a_ajoute_instruction(func_param, temporaire, NULL, NULL, "Paramètre opExp");

    return temporaire;
  }

  operande* etiquette;

  operande* etiq = code3a_new_etiquette_auto();

  etiquette = code3a_new_etiquette(etiq->u.oper_nom);
  code3a_ajoute_instruction(code, op1, op2, etiquette, "Saut test");

  operande *temporaire, *const_vrai, *const_faux, *test;
  temporaire = code3a_new_temporaire();
  const_vrai = code3a_new_constante(1);
  const_faux = code3a_new_constante(0);
  etiq = code3a_new_etiquette_auto();
  test = code3a_new_etiquette(etiq->u.oper_nom);
  code3a_ajoute_instruction(assign, const_faux, NULL, temporaire, "Cas du test faux");
  code3a_ajoute_instruction(jump, test, NULL, NULL, "jump jump jump");
  code3a_ajoute_etiquette(etiquette->u.oper_nom);
  code3a_ajoute_instruction(assign, const_vrai, NULL, temporaire, "Cas du test vrai");
  code3a_ajoute_etiquette(test->u.oper_nom);

  return temporaire;
}

/*-------------------------------------------------------------------------*/

operande* parcours_intExp(n_exp *n)
{
  char texte[ 50 ]; // Max. 50 chiffres
  sprintf(texte, "%d", n->u.entier);

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

operande* parcours_appelExp(n_exp *n)
{
  operande* temporaire = parcours_appel(n->u.appel);
  code3a_ajoute_instruction(func_call, code3a_new_etiquette(n->u.appel->fonction), NULL, temporaire, NULL);

  return temporaire;
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

  char nom[20];
  strcpy(nom, "f");
  strcat(nom, n->nom);

  operande* fonction = code3a_new_etiquette(nom);
  code3a_ajoute_etiquette(fonction->u.oper_nom);
  code3a_ajoute_instruction(func_begin, NULL, NULL, NULL, "Début de la fonction");

  ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_FONCTION, 0, complement);
  entreeFonction();
  parcours_l_dec(n->u.foncDec_.param);
  portee = P_VARIABLE_LOCALE;
  parcours_l_dec(n->u.foncDec_.variables);
  parcours_instr(n->u.foncDec_.corps);

  code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin de la fonction");
  sortieFonction(AFFtabSymboles);
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
                                code3a_ajoute_instruction(alloc, constante, var, NULL, "Alloc Dec var simple");
                                adresseGlobaleCourante += 4;
                                break;
      case P_VARIABLE_LOCALE : address = adresseLocaleCourante;
                               ajouteIdentificateur(n->nom, portee, T_ENTIER, address, complement);
                               var = code3a_new_var(nom, portee, address);
                               constante = code3a_new_constante(complement);
                               code3a_ajoute_instruction(alloc, constante, var, NULL, "Alloc Dec var simple");
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
  char texte[100]; // Max. 100 chars nom tab + taille

  if(rechercheDeclarative(n->nom) == -1) {
    int address;
    int complement = n->u.tabDec_.taille;

    operande* var;

    address = adresseGlobaleCourante;
    ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, address, complement);
    var = code3a_new_var(n->nom, portee, address);
    adresseGlobaleCourante += 4*complement;

    operande* constante;
    constante = code3a_new_constante(complement);
    code3a_ajoute_instruction(alloc, constante, var, NULL, "Alloc Dec var indicée");
  }
  else {
	  erreur("Varaible tableau déjà déclarée");
  }

  sprintf(texte, "%s[%d]", n->nom, n->u.tabDec_.taille);
}

/*-------------------------------------------------------------------------*/

operande* parcours_var(n_var *n)
{
  operande* var;

  if(n->type == simple) {
    var = parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    var = parcours_var_indicee(n);
  }

  return var;
}

/*-------------------------------------------------------------------------*/
operande* parcours_var_simple(n_var *n)  /*********************/
{
  int indice_var;
  if((indice_var = rechercheExecutable(n->nom)) == -1) {
	  erreur("Une variable simple doit être déclarée");
  }

  if(tabsymboles.tab[indice_var].type == T_TABLEAU_ENTIER) {
	erreur("Une variable simple ne doit pas être utilisé avec un indice");
  }

  operande* var = code3a_new_var(n->nom, tabsymboles.tab[indice_var].portee, tabsymboles.tab[indice_var].adresse);

  return var;
}

/*-------------------------------------------------------------------------*/
operande* parcours_var_indicee(n_var *n)   /********************/
{
  int indice_var;
  if((indice_var = rechercheExecutable(n->nom)) == -1) {
	  erreur("Une variable tableau doit être déclarée avant utilisation");
  }

	if(tabsymboles.tab[indice_var].type == T_ENTIER) {
		erreur("Une variable tableau doit être utilisée avec un indice");
	}

  int address = tabsymboles.tab[indice_var].adresse;
  operande* indice = parcours_exp( n->u.indicee_.indice );
  operande* var = code3a_new_var_indicee(n->nom, P_VARIABLE_GLOBALE, address, indice);
  //printf("Type d'operande pour les indices: %d", indice->oper_type);

  return var;
}

/*-------------------------------------------------------------------------*/
int longueur_liste(n_l_dec *l_dec)
{
  if(l_dec == NULL)
    return 0;

  return longueur_liste(l_dec->queue) + 1;
}

int longueur_args(n_l_exp *l_args) {

  if(l_args == NULL){
    return 0;
  }

  operande* arg;

  if(l_args->tete->type == varExp){

    char* nom = l_args->tete->u.var->nom;
    int ligne = rechercheExecutable(nom);
    int adresse = tabsymboles.tab[ligne].adresse;

    arg = code3a_new_var(nom, 3, adresse);
    code3a_ajoute_instruction(func_param, arg, NULL, NULL, "param variable");

  }

  if(l_args->tete->type == intExp){

    int val = l_args->tete->u.entier;

    arg = code3a_new_constante(val);
    code3a_ajoute_instruction(func_param, arg, NULL, NULL, "param constante");
  }

  return longueur_args(l_args->queue) + 1;
}
