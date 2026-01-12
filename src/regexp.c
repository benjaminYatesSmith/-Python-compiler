/**
 * @file regexp.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief regexp
 *
 * regexp code, as in the project's pdf document
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pyas/list.h>
#include <pyas/queue.h> 
#include <pyas/regexp.h>
#include <pyas/chargroup.h>
#include <assert.h>



//-----------------------------
list_t re_read(char* regexp_str) {
    list_t re = list_new();

    if (!regexp_str || *regexp_str == '\0') {
        re = list_add_first(NULL, re);
        return re;
    }
    
    int idx = 0;
    if (regexp_str[idx] == '+' || regexp_str[idx] == '*' || regexp_str[idx] == '?' ) {
        fprintf(stderr, "Erreur: caractère spécial '%c' comme première caractère, index %d.\n", regexp_str[idx], idx);
        list_delete(re, chargroup_delete_cb);
        return NULL;
    }

    while (regexp_str[idx] != '\0') {
        // Crée un nouveau groupe
        chargroup_t cg = chargroup_new();

        if (regexp_str[idx] == '\n' || regexp_str[idx] == '\t' ){
            fprintf(stderr, "Erreur: opérateurs invalides ('\t' ou '\n') à l'index %d.\n", idx);
            chargroup_delete(cg);
            list_delete(re, chargroup_delete_cb);
            return NULL;
        }
        // Vérifie s'il y a une mauvaise combinaison d'opérateurs spéciaux
        if (check_special_operators(regexp_str, idx) != 0) {
            fprintf(stderr, "Erreur: opérateurs spéciaux consécutifs ou invalides à l'index %d.\n", idx);
            chargroup_delete(cg);
            list_delete(re, chargroup_delete_cb);
            return NULL;
        }

        // Gestion du '^' éventuel (négation)
        if (regexp_str[idx] == '^') {
            cg->has_negation = 1;
            idx++;
            if (regexp_str[idx] == '\0') {
                fprintf(stderr, "Erreur: '^' en fin de chaîne sans caractère suivant.\n");
                chargroup_delete(cg);
                list_delete(re, chargroup_delete_cb);
                return NULL;
            }
        }

        // Lecture du bloc : soit un caractère normal, soit '.', soit '[ ]', soit une séquence échappée
        if (regexp_str[idx] == '\\') {
            // parse_escape_sequence
            if (parse_escape_sequence(regexp_str, &idx, cg) != 0) {
                chargroup_delete(cg);
                list_delete(re, chargroup_delete_cb);
                return NULL;
            }
        }
        else if (regexp_str[idx] == '.') {
            // '.' = tous les caractères
            chargroup_add_all_chars(cg);
            idx++;
        }
        else if (regexp_str[idx] == '[') {
            // parse_char_class
            if (parse_char_class(regexp_str, &idx, cg) != 0) {
                chargroup_delete(cg);
                list_delete(re, chargroup_delete_cb);
                return NULL;
            }
        }
        else if(regexp_str[idx] == ']'){
                fprintf(stderr, "Erreur: ']' avant '[' à l'index %d.\n", idx);
                chargroup_delete(cg);
                list_delete(re, chargroup_delete_cb);
                return NULL;
        }   
        else {
            // Caractère normal
            chargroup_add_char(cg, (unsigned char)regexp_str[idx]);
            idx++;
        }

        // Vérifie si le caractère suivant est un opérateur (*, +, ?)
        // parse_operator ne renvoie pas d'erreur, 
        // mais s'il le fallait, on pourrait la gérer.
        parse_operator(regexp_str, &idx, cg);

        // Ajoute le groupe à la liste
        re = list_add_last(re, cg);
    }

    return re;
}
//-----------------------------
int check_special_operators(const char* regexp_str, int idx) {
    /*
      0 = aucun
      1 = opérateur (+, *, ?)
      2 = '^'
    */
    static int prev_type = 0;

    char current = regexp_str[idx];

    // On traite d’abord le cas des opérateurs +, * ou ?
    if (current == '+' || current == '*' || current == '?') {
        // Si le précédent était un opérateur (1), c'est un 2e opérateur de suite => erreur
        // Si le précédent était '^' (2), alors '^' est suivi d'un opérateur => erreur
        if (prev_type == 1 || prev_type == 2) {
            return -1;
        }
        // Sinon, on accepte => on met prev_type = 1
        prev_type = 1;
    }
    // Ensuite, le cas du '^'
    else if (current == '^') {
        // Si le précédent était '^' => erreur (^^)
        // Si le précédent était un opérateur => on autorise (ex: *^)
        if (prev_type == 2) {
            return -1;
        }
        // On met prev_type = 2
        prev_type = 2;
    }
    // Sinon, c'est un caractère normal
    else {
        prev_type = 0;
    }

    return 0; // Pas d'erreur
}

//-----------------------------
int parse_char_class(const char* regexp_str, int* idx_ptr, chargroup_t cg) {
    /*
      Parse jusqu'à la fermeture ']', gère les intervalles x-y,
      refuse +, *, ?, ^ s'ils ne sont pas échappés,
      et autorise (a\-z) => "a", "-", "z".
    */
    int idx = *idx_ptr;

    idx++; // passer le '['
    if (regexp_str[idx] == '\0') {
        fprintf(stderr, "Erreur: '[' non fermé (fin de chaîne atteinte)\n");
        return -1;
    }
    if(regexp_str[idx] == ']'){
        fprintf(stderr, "Erreur: aucune information entre les crochets '[]'\n");
        return -1;
    }
    if(regexp_str[idx] == '-'){
        fprintf(stderr, "Erreur: aucuns caractères avant le tiret '[ -...]'\n");
        return -1;
    }

    while (regexp_str[idx] != ']') {
        if (regexp_str[idx] == '\0') {
            fprintf(stderr, "Erreur: ']' manquant pour fermer la classe de caractères.\n");
            return -1;
        }
        if (regexp_str[idx] == '\n' || regexp_str[idx] == '\t' ){
            fprintf(stderr, "Erreur: opérateurs invalides ('\t' ou '\n') dans '[...]'.\n");
            return -1;
        }

        // Si on a un backslash => on considère le caractère qui suit comme littéral
        if (regexp_str[idx] == '\\') {
            idx++;
            if (regexp_str[idx] == '\0') {
                fprintf(stderr, "Erreur: '\\' sans caractere suivant à l'index %d\n", idx);
                return -1;
            }
            if (regexp_str[idx] == 'n')
                chargroup_add_char(cg, '\n');
            else if(regexp_str[idx] == 't')
                chargroup_add_char(cg, '\t');
            else{ 
            // On ajoute le caractère échappé tel quel
                chargroup_add_char(cg, (unsigned char)regexp_str[idx]);}
            idx++;
            continue;
        }

        // Vérifie si c'est un intervalle x-y
        // On regarde s'il y a un '-' après le caractère courant,
        // et un 3e caractère après différent de ']' et != '\0'.
        if (regexp_str[idx + 1] == '-' && regexp_str[idx + 2] != '\0'){
            if(regexp_str[idx + 2] == ']'){
                fprintf(stderr, "Erreur: aucuns caractères apres le tiret '[... - ]'\n");
                return -1;
            }
            char start = regexp_str[idx];
            char end = regexp_str[idx + 2];

            // On ajoute tous les chars de [start..end]
            if (start > end) {
                fprintf(stderr, "Erreur: intervalle inversé '%c-%c' à l'index %d.\n", start, end, idx);
                return -1;
            }
            for (int c = start; c <= end; c++) {
                chargroup_add_char(cg, (unsigned char)c);
            }
            idx += 2; 
            if(regexp_str[idx-1] =='-' && regexp_str[idx+1] =='-'){ //traite les cas comme [a-b-z]
                fprintf(stderr, "Erreur: 'trop de '-' à l'index %d\n", idx);
                return -1; 
            }
            idx++; // On a consommé start, '-', end
            continue;
        }

        // Si c'est un caractère spécial +, *, ?, ^ NON échappé => erreur
        if (regexp_str[idx] == '+' || regexp_str[idx] == '*' || regexp_str[idx] == '?' || regexp_str[idx] == '^' || regexp_str[idx] == '[' || regexp_str[idx] == '.' ) {
            fprintf(stderr, "Erreur: caractère '%c' qui n'ont pas lieu d'etre dans '[]' (non échappe), index %d.\n", regexp_str[idx], idx);
            return -1;
        }

        // Sinon, on ajoute le caractère normalement
        chargroup_add_char(cg, (unsigned char)regexp_str[idx]);
        idx++;
    }

    idx++; // passer le ']'
    *idx_ptr = idx; // Mettre à jour l'index
    return 0; // Succès
}

//-----------------------------
int parse_escape_sequence(const char* regexp_str, int* idx_ptr, chargroup_t cg) {
    /*
      Gère \n, \t, et sinon ajoute le caractère littéral.
    */
    int idx = *idx_ptr;

    if (regexp_str[idx + 1] == '\0') {
        fprintf(stderr, "Erreur: '\\' sans caractère suivant à l'index %d\n", idx);
        return -1;
    }

    idx++; // Passer le '\'

    switch (regexp_str[idx]) {
        case 'n':
            chargroup_add_char(cg, '\n');
            break;
        case 't':
            chargroup_add_char(cg, '\t');
            break;
        default:
            // Cas général
            chargroup_add_char(cg, (unsigned char)regexp_str[idx]);
            break;
    }

    idx++; // Avancer après le caractère échappé
    *idx_ptr = idx;
    return 0;
}

//-----------------------------
int parse_operator(const char* regexp_str, int* idx_ptr, chargroup_t cg) {
    /*
      Retourne 1 si un opérateur est lu, 0 sinon
    */
    int idx = *idx_ptr;

    if (regexp_str[idx] == '*' || regexp_str[idx] == '+' || regexp_str[idx] == '?') {
        switch (regexp_str[idx]) {
            case '*':
                chargroup_set_operator_star(cg);
                break;
            case '+':
                chargroup_set_operator_plus(cg);
                break;
            case '?':
                chargroup_set_operator_question(cg);
                break;
        }
        idx++;
        *idx_ptr = idx;
        return 1;
    }
    return 0;
}

//-----------------------------

static int    list_print_read( list_t l, action_t print ) {
  /* A printing function should return the number of characters it
     has printed.
  */
  int ret = 0; //printf( "%s", list_is_empty( l ) ? "" : " " );

  for ( ; !list_is_empty( l ) ; l = list_next( l ) ) {
    ret += print ? print( list_first( l ) ) : printf( "#OBJECT#" );
    ret += printf( "\n" );
  }

  return ret;
}


void re_print(list_t regexp) {
    // affiche une expression régulière encodée sous la forme d'une liste de chargroup_t
    // PRECONDITION : regexp non vide
    assert(regexp != NULL);
    list_print_read(regexp, chargroup_print_cb); 
}


