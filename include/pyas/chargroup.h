/**
 * @file regexp.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief General include for regexp
 *
 * General include for regexp, as in the project's pdf document
 */

#ifndef _CHARGROUP_H_
#define _CHARGROUP_H_

#ifdef __cplusplus
extern "C" {
#endif

// @TODO Faire un type opaque...
struct chargroup {
  char set[128]; // ou char set[256] . Un tableau de booléen indexé par un caractère
  int has_star_operator; // booléen. Vrai si operateur * activé. A NOTER : à faire évoluer plus tard !
  int has_plus_operator;
  int has_question_operator;
  int has_negation; // '^'
};

typedef struct chargroup * chargroup_t;

// Alloue un groupe de caractère, initialement
// sans caractère et sans operateur *
chargroup_t chargroup_new();

// Libère la mémoire d'un groupe de caractère
// PRECONDITION cg != NULL
void chargroup_delete(chargroup_t cg);

// Affiche la chargroup, au format donné dans le sujet
// Les caractères spéciaux (comme BELL, code ASCII 7)
// seront affichés avec le symbole € (code ASCI 128)
// PRECONDITION cg != NULL
void chargroup_print(chargroup_t cg);

// Ajoute le caractère de code ASCII c dans le chargroup cg
// PRECONDITION cg != NULL
// PRECONDITION : 0<=c
void chargroup_add_char(chargroup_t cg, char c);

// Ajoute tous les caractères (ASCII 0 à 128) du chargroup cg
// PRECONDITION cg != NULL
// (utile pour l'opérateur point ".")
void chargroup_add_all_chars(chargroup_t cg);

// Retourne "vrai" (1) si le caractère c est dans le chargroup, "faux" (0) sinon
// PRECONDITION cg != NULL
// PRECONDITION : 0<=c
int chargroup_has_char(chargroup_t cg, char c);

// Allume l'opérateur * ("zero or more")
// PRECONDITION cg != NULL
void chargroup_set_operator_star(chargroup_t cg);

// Retourne True (1) si l'opérateur * est activé dans le chargroup cg, False sinon
// PRECONDITION cg != NULL
int chargroup_has_operator_star(chargroup_t cg);

// ... etc
// autres prototypes de fonction à réfléchir

////////
// Fonctions callback pour le cas ou des chargroup
// sont placés dans des listes chaînées génériques
////////

// Fonction "callback" de libération mémoire
int chargroup_delete_cb(void * cg);

// Fonction "callback" d'affichage d'un groupe de caractères
int chargroup_print_cb(void * cg);

// Met le code ASCI de l'opérateur + à 1
void chargroup_set_operator_plus(chargroup_t cg);

// Vérifie que le chargroup possède l'opérateur + en analysant les 128 caractères ASCI, retourne 1 ou 0
int chargroup_has_operator_plus(chargroup_t cg);

// Met le code ASCI de l'opérateur ? à 1
void chargroup_set_operator_question(chargroup_t cg);

// Vérifie que le chargroup possède l'opérateur ? en analysant les 128 caractères ASCI, retourne 1 ou 0
int chargroup_has_operator_question(chargroup_t cg);

// Vérifie que le chargroup n'est pas vide 
int chargroup_not_empty(chargroup_t cg);

#ifdef __cplusplus
}
#endif

#endif /* _ALL_H_ */