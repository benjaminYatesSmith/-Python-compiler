#include <stdlib.h>
#include <stdio.h>

#include <pyas/regexp.h>
#include <pyas/list.h>
#include <pyas/chargroup.h>


int main(int argc, char *argv[]) {
    // Vérifie que le programme a reçu exactement un argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <regexp>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // L'argument unique est l'expression régulière
    char *regexp = argv[1];

    // Lis l'expression régulière et construit la structure de données
    list_t re = re_read(regexp);

    // Vérifie si l'expression régulière est valide
    if (re == NULL) {
        fprintf(stderr, "Error: Invalid regular expression\n");
        exit(EXIT_FAILURE);
    }

    // Affiche la structure de l'expression régulière
    re_print(re);

    // Libérons la mémoire
    list_delete(re, chargroup_delete_cb);

    exit(EXIT_SUCCESS);
}