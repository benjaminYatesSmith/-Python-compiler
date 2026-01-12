#include <stdlib.h>
#include <stdio.h>

#include <pyas/list.h>
#include <pyas/lexem.h>
#include <pyas/regexp.h>

int main(int argc, char *argv[])
{
    // Vérifie que le programme a reçu exactement 3 argument
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <lexer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    list_t lex_list = lex(argv[1], argv[2]);
    if( NULL == lex_list) exit(EXIT_FAILURE);
    list_print(lex_list, lexem_print);

    exit(EXIT_SUCCESS);
}
