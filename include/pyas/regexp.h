/**
 * @file regexp.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief General include for regexp
 *
 * General include for regexp, as in the project's pdf document
 */

#ifndef _REGEXP_H_
#define _REGEXP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pyas/list.h> 
#include <pyas/chargroup.h>

  //Lire une expression régulière “simplifiée” et la convertir en liste de chargroup_t.
  list_t re_read(char* regexp_str);


  // affiche une expression régulière encodé sous la forme d'une liste de chargroup_t
  void re_print(list_t re);
  // Vérifie que la syntaxe des opérateurs est correcte ("Erreur: opérateurs spéciaux consécutifs ou invalides à l'index)
  int check_special_operators(const char* regexp_str, int idx);
  //  Parse jusqu'à la fermeture ']', gère les intervalles x-y, refuse +, *, ?, ^ s'ils ne sont pas échappés,et autorise (a\-z) => "a", "-", "z".   
  int parse_char_class(const char* regexp_str, int* idx_ptr, chargroup_t cg);
  //  Gère \n, \t, et sinon ajoute le caractère littéral.
  int parse_escape_sequence(const char* regexp_str, int* idx_ptr, chargroup_t cg);
  //  Retourne 1 si un opérateur est lu, 0 sinon
  int parse_operator(const char* regexp_str, int* idx_ptr, chargroup_t cg);



#ifdef __cplusplus
}
#endif

#endif /* _ALL_H_ */
