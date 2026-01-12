/**
 * @file chargroup.c
 * @author NC
 * @brief manipulation des gropupes de caractères
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <pyas/chargroup.h>

chargroup_t chargroup_new() {
  chargroup_t cg = NULL;
  cg = calloc(1, sizeof(*cg));
  if( NULL == cg) {
    fprintf(stderr, "Erreur grave 875***\n");
    exit(1);
  }
  return cg;
}

void chargroup_delete(chargroup_t cg) {
  assert( NULL != cg );
  free(cg);
}

void chargroup_print(chargroup_t cg) {
  
  assert( NULL != cg );
  if(cg->has_negation) 
    printf("One not in \"");
  else
    printf("One in \"");
  for(int i = 0 ; i < 128 ; i++) {
    if( cg->set[i] ) {
      if( isgraph( i ) ) {
        printf("%c", i);
      } else if(i == '\n') {
        printf("\\n");
      } else if(i == '\t') {
        printf("\\t");
      } else if(i == ' ') {
        printf(" ");
      } else {
        printf("€");
      }
    }
  }
  printf("\", ");
  if( cg->has_star_operator ) {
    printf("zero or more times.");}
  else if( cg->has_plus_operator ) {
    printf("one or more times.");}
  else if( cg->has_question_operator ) {
    printf("zero or one time.");}
  else {
    printf("one time.");
  }
}

void chargroup_add_char(chargroup_t cg, char c) {
  assert( NULL != cg );
  assert( c >= 0);
  cg->set[ (int) c] = 1;
}

void chargroup_add_all_chars(chargroup_t cg) {
  assert( NULL != cg );
  for(int i = 0 ; i < 128 ; i++) {
    cg->set[i] = 1;
  }
}

int chargroup_has_char(chargroup_t cg, char c) {
  assert( NULL != cg );
  assert( c >= 0);
  return cg->set[ (int)c ];
}

void chargroup_set_operator_star(chargroup_t cg) {
  assert( NULL != cg );
  cg->has_star_operator = 1;
}

int chargroup_has_operator_star(chargroup_t cg) {
  assert( NULL != cg );
  return cg->has_star_operator;
}

void chargroup_set_operator_plus(chargroup_t cg) {
  assert( NULL != cg );
  cg->has_plus_operator = 1;
}

int chargroup_has_operator_plus(chargroup_t cg) {
  assert( NULL != cg );
  return cg->has_plus_operator;
}

void chargroup_set_operator_question(chargroup_t cg) {
  assert( NULL != cg );
  cg->has_question_operator = 1;
}

int chargroup_has_operator_question(chargroup_t cg) {
  assert( NULL != cg );
  return cg->has_question_operator;
}


// fonction callback de suppression
int chargroup_delete_cb(void * cg) {
  assert( NULL != cg );
  chargroup_delete((chargroup_t)cg);
  return 0;
}

// fonction callback d'impression
int chargroup_print_cb(void * cg) {
  assert( NULL != cg );
  chargroup_print((chargroup_t)cg);
  return 0;
}


int chargroup_not_empty(chargroup_t cg) {
    // Vérifie qu'au moins un élément est non nul

    for (int i = 0; i < 128; i++) {
        if (chargroup_has_char(cg, i)) {
            return 1; // Au moins un élément est non nul
        }
    }
    return 0; // Tous les éléments sont nuls
}