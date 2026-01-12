#include <stdio.h>
#include <stdlib.h>

#include <pyas/list.h>
#include <pyas/lexem.h>
#include <pyas/parse.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <parser>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    list_t lexems = lex("regexp_file.txt", argv[1]);  // par exemple
    pyobj_t ast = parse(&lexems);
    if( NULL == ast ) exit(EXIT_FAILURE);
    print_pyobj(ast);

    exit(EXIT_SUCCESS);
}
