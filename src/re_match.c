#include <stdio.h> // Include standard I/O if necessary
#include <stdlib.h>
#include <pyas/re_match.h>
#include <pyas/chargroup.h>
#include <pyas/list.h>

static int re_match_zero_or_more( chargroup_t cg, char **end ) {
  if (cg->has_negation) {
    while (**end != '\0' && !chargroup_has_char(cg, **end)) (*end)++;
  }
  else {
    while (**end != '\0' && chargroup_has_char(cg, **end)) (*end)++;
  }
  return 1;
}

static int re_match_zero_or_one( chargroup_t cg, char **end ) {
  if (cg->has_negation) {
    if (**end != '\0' && !chargroup_has_char(cg, **end)) (*end)++;
  }
  else {
    if (**end != '\0' && chargroup_has_char(cg, **end)) (*end)++;
  }
  return 1;
}

static int re_match_one_or_more( chargroup_t cg, char **end ) {
  if (cg->has_negation) {
    if (chargroup_has_char(cg, **end)) return 0; 
    while (**end != '\0' && !chargroup_has_char(cg, **end)) (*end)++;
    return 1;
  }
  else {
    if (!chargroup_has_char(cg, **end)) return 0;
    while (**end != '\0' && chargroup_has_char(cg, **end)) (*end)++;
    return 1;
  }
}

int re_match(list_t re, char *source, char **end) {
  if (!re || !source) {
    fprintf(stderr, "re or match is NULL, please check the arguments of re_match");
    return 0;
  }

  char *current = source;
  chargroup_t cg = NULL;
  int matched = 0;

  if (!list_first(re)) {
    return 1;
  }

  while (!list_is_empty(re)) {
    cg = list_first(re);
    re = list_next(re);

    // Gestion des opérateurs
    if (chargroup_has_operator_star(cg)) {
      matched = re_match_zero_or_more(cg, &current);
    } else if (chargroup_has_operator_question(cg)) {
      matched = re_match_zero_or_one(cg, &current);
    } else if (chargroup_has_operator_plus(cg)) {
      matched = re_match_one_or_more(cg, &current);
    } else {
      if (!cg->has_negation) {
        if (*current != '\0' && chargroup_has_char(cg, *current)) {
          current++;
          matched = 1;
        } else {
          return 0;
        }
      }
      else {
        if (*current != '\0' && !chargroup_has_char(cg, *current)) {
          current++;
          matched = 1;
        } else {
          return 0;
        }
      }
    }

    if (!matched) {
      return 0;  // Aucun match trouvé pour ce groupe
    }
  }

  if (end != NULL) {
    *end = current;  // Mise à jour du pointeur de fin si non NULL
  }

  return 1;  // Tous les groupes ont été vérifiés avec succès
}


