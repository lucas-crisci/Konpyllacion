#include <stdio.h>
#include "syntabs.h"
#include "util.h"
#include "tabsymboles.h"
#include "code3a.h"


	// a + 3 donne enfaite
	// operande *t1 = parcours_exp(n->op1);
	// operande *t2 = parcours_exp(n->op2);
	// operande *t3 = code3code3a_new_temporaire();
	// code3a_ajoute_instruction(arith_add,t1,t2,t3);
	// return t3



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
void parcours_exp(n_exp *n);
void parcours_varExp(n_exp *n);
void parcours_opExp(n_exp *n);
void parcours_intExp(n_exp *n);
void parcours_lireExp(n_exp *n);
void parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
void parcours_var(n_var *n);
void parcours_var_simple(n_var *n);
void parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);
int nb_param(n_l_dec* liste);

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
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

void parcours_instr(n_instr *n)
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
	operande *suite = code3a_new_etiquette("SUITE");
	operande *faux = code3a_new_etiquette("FAUX");

  operande *res = parcours_exp(n->u.si_.test);

	code3a_ajoute_instruction(jump_if_equal,res,code3a_new_constante(0),faux);
	parcours_instr(n->u.si_.alors);
	code3a_ajoute_instruction(jump,suite,NULL,NULL);

	code3code3a_ajoute_etiquette(faux->u.oper_nom);

  if(n->u.si_.sinon){
    parcours_instr(n->u.si_.sinon);
  }
	code3code3a_ajoute_etiquette(suite->u.oper_nom);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
	operande *suite = code3a_new_etiquette("SUITE");
	operande *test = code3a_new_etiquette("test");

	code3code3a_ajoute_etiquette(test->u.oper_nom);
  operande *res = parcours_exp(n->u.tantque_.test);

	code3a_ajoute_instruction(jump_if_equal,res,code3a_new_constante(0),suite);
  parcours_instr(n->u.tantque_.faire);
	code3a_ajoute_instruction(jump,test,NULL,NULL);

	code3code3a_ajoute_etiquette(suite->u.oper_nom);


}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  parcours_var(n->u.affecte_.var);
  parcours_exp(n->u.affecte_.exp);


}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)
{
  parcours_l_exp(n->args);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{

  parcours_exp(n->u.retour_.expression);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
	code3a_ajoute_instruction(sys_write,n->u.ecrire_.expression,NULL,NULL,NULL);
  parcours_exp(n->u.ecrire_.expression);





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

void parcours_exp(n_exp *n)
{
  if(n->type == varExp) parcours_varExp(n);
  else if(n->type == opExp) parcours_opExp(n);
  else if(n->type == intExp) parcours_intExp(n);
  else if(n->type == appelExp) parcours_appelExp(n);
  else if(n->type == lireExp) parcours_lireExp(n);
}

/*-------------------------------------------------------------------------*/

void parcours_varExp(n_exp *n)
{
  parcours_var(n->u.var);
}

/*-------------------------------------------------------------------------*/
void parcours_opExp(n_exp *n)
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
    printf("10oper_type = %d\n", temporaire->oper_type);
    code3a_ajoute_instruction(code, op1, op2, temporaire, "Arithmétique / Logique");

    return temporaire;
  }

  operande* etiquette;
  etiquette = code3a_new_etiquette(new_e());
  printf("11oper_type = %d\n", etiquette->oper_type);
  code3a_ajoute_instruction(code, op1, op2, etiquette, "Saut test");

  operande *temporaire, *const_vrai, *const_faux, *test;
  temporaire = code3a_new_temporaire();
  printf("12oper_type = %d\n", temporaire->oper_type);
  const_vrai = code3a_new_constante(1);
  printf("13oper_type = %d\n", const_vrai->oper_type);
  const_faux = code3a_new_constante(0);
  printf("14oper_type = %d\n", const_faux->oper_type);
  test = code3a_new_etiquette(new_e());
  printf("15oper_type = %d\n", test->oper_type);
  code3a_ajoute_instruction(assign, const_faux, NULL, temporaire, "Cas du test faux");
  code3a_ajoute_instruction(jump, test, NULL, NULL, NULL);
  code3a_ajoute_etiquette(etiquette->u.oper_nom);
  code3a_ajoute_instruction(assign, const_vrai, NULL, temporaire, "Cas du test vrai");
  code3a_ajoute_etiquette(test->u.oper_nom);

  return temporaire;
}

/*-------------------------------------------------------------------------*/

void parcours_intExp(n_exp *n)
{
	code3a_ajoute_instruction(
	alloc,
	1,
	code3a_new_constante(n),
	NULL,
	NULL);
}

/*-------------------------------------------------------------------------*/
void parcours_lireExp(n_exp *n)
{
	code3a_ajoute_instruction(sys_read,NULL,NULL,NULL,NULL);
}

/*-------------------------------------------------------------------------*/

void parcours_appelExp(n_exp *n)
{

	code3a_ajoute_instruction(func_call,NULL,NULL,NULL,NULL)
  parcours_appel(n->u.appel);
}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  if( n ){
    parcours_dec(n->tete);
    parcours_l_dec(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)
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
int nb_param(n_l_dec* liste){
	int n = 0;
	while(liste != NULL){
		n++;
		liste = liste->queue;
	}
	return n;
}

void parcours_foncDec(n_dec *n)
{
	if(rechercheDeclarative(n->nom) ==-1 ){

		int nbparam = nb_param (n->u.foncDec_.param);
		ajouteIdentificateur(n->nom,portee,T_FONCTION,0, nbparam);
		tabsymboles.base++;

		entreeFonction();
		code3a_new_etiquette(n->nom);
		code3a_ajoute_instruction(func_begin,NULL,NULL,NULL,NULL);



		parcours_l_dec(n->u.foncDec_.param);
    portee = P_VARIABLE_LOCALE;
		parcours_l_dec(n->u.foncDec_.variables);
    portee = P_VARIABLE_GLOBALE;
		parcours_instr(n->u.foncDec_.corps);
		code3a_ajoute_instruction(func_end,NULL,NULL,NULL,NULL);
		sortieFonction(1);

	}
	else
		erreur("Already defined");
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{
  if (rechercheExecutable(n->nom) == -1) {
    if (portee == P_VARIABLE_GLOBALE)
    {
        ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseLocaleCourante, 1);
        tabsymboles.base++;
				code3a_ajoute_instruction(
					alloc,
					1,
					code3a_new_var(n->nom,portee,adresseLocaleCourante),
					NULL,
					NULL);
        adresseLocaleCourante= adresseLocaleCourante +4;

    }
    else if (portee == P_VARIABLE_LOCALE)
    {
        ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseLocaleCourante, 1);
				code3a_ajoute_instruction(
					alloc,
					1,
					code3a_new_var(n->nom,portee,adresseLocaleCourante),
					NULL,
					NULL);
      adresseLocaleCourante= adresseLocaleCourante +4;

    }
    else if (portee == P_ARGUMENT)
    {
      ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseArgumentCourant, 1);
			code3a_ajoute_instruction(
				alloc,
				1,
				code3a_new_var(n->nom,portee,adresseLocaleCourante),
				NULL,
				NULL);
      adresseArgumentCourant=adresseArgumentCourant+4;
    }

  }
  else erreur("Var undefined");
}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{
  int result = rechercheDeclarative(n->nom);
  if (result == -1) {
    if (portee != P_VARIABLE_GLOBALE)
		erreur("tab defined as global ");
    ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, adresseLocaleCourante, n->u.tabDec_.taille);

		code3a_ajoute_instruction(
			alloc,
			n->u.tabDec_.taille,
			code3a_new_var(n->nom,portee,adresseLocaleCourante),
			NULL,
			NULL);

    tabsymboles.base++;
    adresseLocaleCourante += n->u.tabDec_.taille*4;
  }
  else erreur("Tab already defined");
}

/*-------------------------------------------------------------------------*/

void parcours_var(n_var *n)
{
  if(n->type == simple) {
    parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    parcours_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_var_simple(n_var *n)
{
	code3a_ajoute_instruction(
		alloc,
		1,
		code3a_new_constante(n),
		NULL,
		NULL);


  if (rechercheExecutable(n->nom) == -1)
	  erreur("var undefined");

}

/*-------------------------------------------------------------------------*/

void parcours_var_indicee(n_var *n)
{
  if (rechercheExecutable(n->nom) == -1)
	  erreur("Var undefined");
  else if (tabsymboles.tab[rechercheExecutable(n->nom)].type == T_ENTIER)
	  erreur("IT'S NOT A TAB!");

}
