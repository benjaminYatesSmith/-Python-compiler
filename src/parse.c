#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include <pyas/lexem.h>
#include <pyas/parse.h>

/* Structures de données (rappel, simplifiées) */

#define NULL_MARKER           '0'  // Absence d’objet
#define NONE_MARKER           'N'  // None
#define FALSE_MARKER          'F'  // False
#define TRUE_MARKER           'T'  // True
#define INT_MARKER            'i'  // Entier signé sur 4 octets
#define INT64_MARKER          'I'  // Entier signé sur 8 octets (plus généré)
#define FLOAT_MARKER          'f'  // Chaîne d’un réel (max : 17 caractères)
#define BINARY_FLOAT_MARKER   'g'  // Réel binaire sur 8 octets (double)
#define COMPLEX_MARKER        'x'  // Deux chaînes pour un complexe
#define BINARY_COMPLEX_MARKER 'y'  // Deux réels pour un complexe
#define STRING_MARKER         's'  // Chaîne
#define STRINGREF_MARKER      'R'  // Référence à une chaîne internée
#define TUPLE_MARKER          '('  // Tuple
#define LIST_MARKER           '['  // Liste
#define DICT_MARKER           '{'  // Dictionnaire
#define SET_MARKER            '<'  // Ensemble
#define CODE_MARKER           'c'  // Objet de code
#define STOP_ITER_MARKER      'S'  // Arrêt d’un itérateur
#define ELLIPSIS_MARKER       '.'  // ...
#define LONG_MARKER           'l'  // Entier signé en base 15
#define UNICODE_MARKER        'u'  // Chaîne Unicode
#define INTERNED_MARKER       't'  // Chaîne Unicode internée
#define UNKNOWN_MARKER        '?'  // Objet de type inconnu
#define FROZENSET_MARKER      '>'  // Ensemble en lecture seule

#define OPTIMIZED               0x0001  // Optimisation active
#define NEWLOCALS               0x0002  // Création d’un nouvel espace local de variables
#define VARARGS                 0x0004  // Fonction acceptant des arguments variables (*args)
#define VARKEYWORDS             0x0008  // Fonction acceptant des arguments mots-clés (**kwargs)
#define NESTED                  0x0010  // Fonction imbriquée
#define GENERATOR               0x0020  // Fonction génératrice
#define NOFREE                  0x0040  // Pas de variables libres
#define COROUTINE               0x0080  // Fonction coroutine
#define ITERABLE_COROUTINE      0x0100  // Coroutine itérable
#define ASYNC_GENERATOR         0x0200  // Générateur asynchrone
#define FUTURE_DIVISION         0x20000 // Division avec comportement Python 3 (/ pour float, // pour int)
#define FUTURE_ABSOLUTE_IMPORT  0x40000 // Importations absolues par défaut
#define FUTURE_WITH_STATEMENT   0x80000 // Utilisation du "with"
#define FUTURE_PRINT_FUNCTION   0x100000 // Utilisation de "print()" comme fonction
#define FUTURE_UNICODE_LITERALS 0x200000 // Les littéraux sont des chaînes Unicode par défaut
#define FUTURE_BARRY_AS_BDFL    0x400000 // Fonctionnalité humoristique "BARRY"
#define FUTURE_GENERATOR_STOP   0x800000 // Stop itérateur sur la fin du générateur (évite StopIteration)
#define FUTURE_ANNOTATIONS      0x1000000 // Support des annotations différées (PEP 563)

typedef unsigned int pyobj_type ;

/* Déclaration avant usage */
struct py_codeblock;

/* Structure globale pyobj_t */
struct pyobj {
    pyobj_type type;
    unsigned int refcount;

    union {
        /* Pour stocker des sous-champs sous forme de liste de pyobj_t */
        struct {
            pyobj_t *value;
            int size;
        } list;

        /* Pour stocker une chaîne */
        struct {
            char *buffer;
            int length;
        } string;

        /* Pour stocker un bloc de code complet (champ unique) */
        struct py_codeblock *codeblock;

        /* Pour stocker un nombre, etc. */
        union {
            int integer;
            int64_t integer64;
            double real;
            struct {
                double real;
                double imag;
            } complex;
        } number;

    } py;
};

/* Codeblock : structure utilisée dans le champ union->codeblock */

typedef struct py_codeblock {
    int version_pyvm;
    struct {
        int arg_count;
        int local_count;
        int stack_size;
        int flags;
    } header;

    pyobj_t parent; // non-géré ici

    struct {
        struct {
            int magic;       // cst
            time_t timestamp;// à ajouter
            int source_size;
        } header;

        struct {
            pyobj_t interned;
            pyobj_t bytecode; // contiendra le « code »
            pyobj_t consts;
            pyobj_t names; 
            pyobj_t varnames; 
            pyobj_t freevars;
            pyobj_t cellvars;
        } content;

        struct {
            pyobj_t filename;
            pyobj_t name;
            int firstlineno; 
            pyobj_t lnotab;
        } trailer;
    } binary;
} py_codeblock;

/* Gestion d'erreurs de parsing */

void print_parse_error( char *msg, list_t *lexems ) {
    /* On admet que la chaîne de caractère msg contient
       exactement deux fois le motif %d */
    int line = lexem_line(lexem_peek(lexems));
    int col  = lexem_col (lexem_peek(lexems));

    fprintf(stderr, msg, line, col);
}

/* Fonctions utilitaires pour construire un pyobj_t */

/* Alloue un pyobj et l’initialise à zéro */
static pyobj_t new_pyobj(pyobj_type type) {
    pyobj_t obj = calloc(1, sizeof(struct pyobj));
    obj->type = type;
    obj->refcount = 1; // si on gère un refcount
    return obj;
}

/* Alloue un pyobj de type STRING_NODE et y stocke la chaîne. */
static pyobj_t new_string_obj(const char *s, pyobj_type type) {
    pyobj_t obj = new_pyobj(type);
    obj->py.string.length = (int)strlen(s);
    obj->py.string.buffer = strdup(s);
    return obj;
}

/* Alloue un pyobj de type INTEGER_NODE */
static pyobj_t new_int_obj(int val, pyobj_type type) {
    pyobj_t obj = new_pyobj(type);
    obj->py.number.integer = val;
    return obj;
}

/* Alloue un pyobj de type FLOAT_NODE */
static pyobj_t new_float_obj(double val) {
    pyobj_t obj = new_pyobj(FLOAT_MARKER);
    obj->py.number.real = val;
    return obj;
}

/* Construit un pyobj liste contenant n enfants */
static pyobj_t new_list_obj(pyobj_type type, int n) {
    pyobj_t obj = new_pyobj(type);
    obj->py.list.size = n;
    if(n > 0) {
        obj->py.list.value = calloc(n, sizeof(pyobj_t));
    }
    return obj;
}

/* Déclarations des sous-fonctions du parseur */

static void   parse_eol_star(list_t *lexems);
static int   parse_prologue(list_t *lexems, py_codeblock *codeblock);
static int   parse_set_directives(list_t *lexems, py_codeblock *codeblock);
static int   parse_set(list_t *lexems, char *set, py_codeblock *codeblock);
static int   parse_interned_strings(list_t *lexems, py_codeblock *codeblock);
static int   parse_constants(list_t *lexems, py_codeblock *codeblock);
static int   parse_optional(list_t *lexems, char *opt, py_codeblock *codeblock); 
static pyobj_t parse_constant(list_t *lexems);
static pyobj_t parse_tuple_or_list(list_t *lexems);
static pyobj_t parse_code(list_t *lexems, py_codeblock *codeblock);
static int parse_assembly_line(list_t *lexems, char *code, int *count1, char *lnt, int *count2);
static pyobj_t parse_function(list_t *lexems);
static void free_pyobj_rec(pyobj_t obj);

/* Implémentation principale : parse() */

pyobj_t parse(list_t *lexems) 
{
    // 1) Créer l’objet racine PYS_NODE
    pyobj_t root = new_pyobj(CODE_MARKER);

    // 2) Allouer le codeblock qui contiendra toutes les informations
    py_codeblock *cb = calloc(1, sizeof(py_codeblock));
    root->py.codeblock = cb;

    cb->binary.trailer.firstlineno = 0;

    // 3) ⟨pys⟩ := ⟨eol⟩* ⟨prologue⟩ ⟨code⟩

    parse_eol_star(lexems);       // Consomme les eol initiaux
    if (!parse_prologue(lexems, cb)) return NULL;   // Prologue
    {
        // Code
        pyobj_t code_obj = parse_code(lexems, cb);
        if (!code_obj) return NULL;
        cb->binary.content.bytecode = code_obj;
    }

    // On pourrait vérifier ici qu’il ne reste plus de lexèmes non-comment/blank, etc.

    return root;
}

static int parse_optional(list_t *lexems, char *opt, py_codeblock *codeblock) {
    if(next_lexem_is(lexems, opt)) {
        lexem_advance(lexems);
        parse_eol_star(lexems);

        pyobj_t opt_node = new_pyobj(SET_MARKER);

        #define MAX_OPT 2048
        pyobj_t strings[MAX_OPT];
        int count = 0;

        while(next_lexem_is(lexems, "string")) {
            lexem_t lx = list_first(*lexems);
            strings[count++] = new_string_obj(lexem_value(lx), STRING_MARKER);
            if(count >= MAX_OPT) {
                print_parse_error("Erreur: trop de chaînes optionnelles (ligne %d col %d)\n", lexems);
                return 0;
            }
            lexem_advance(lexems);
            parse_eol_star(lexems);
        }

        opt_node->py.list.size = count;
        opt_node->py.list.value = calloc(count, sizeof(pyobj_t));
        for(int i=0; i<count; i++) {
            opt_node->py.list.value[i] = strings[i];
        }

        if (!strcmp("dir::names", opt)) codeblock->binary.content.names = opt_node;
        else if (!strcmp("dir::varnames", opt)) codeblock->binary.content.varnames = opt_node;
        else if (!strcmp("dir::freevars", opt)) codeblock->binary.content.freevars = opt_node;
        else if (!strcmp("dir::cellvars", opt)) codeblock->binary.content.cellvars = opt_node;
    }

    return 1;
}

/* Consomme zéro ou plus "newline" */
static void parse_eol_star(list_t *lexems) {
    while(next_lexem_is(lexems, "newline") 
       || next_lexem_is(lexems, "blank")
       || next_lexem_is(lexems, "comment")) 
    {
        lexem_advance(lexems);
    }
}

/* prologue = set-directives + interned-strings + constants 
+ [names] + [varnames] + [freevars] + [cellvars] */
static int parse_prologue(list_t *lexems, py_codeblock *codeblock) {
    if (!parse_set_directives(lexems, codeblock)) return 0;
    if (!parse_interned_strings(lexems, codeblock)) return 0;
    if (!parse_constants(lexems, codeblock)) return 0;
    if (!parse_optional(lexems, "dir::names", codeblock)) return 0;
    if (!parse_optional(lexems, "dir::varnames", codeblock)) return 0;
    if (!parse_optional(lexems, "dir::freevars", codeblock)) return 0;
    if (!parse_optional(lexems, "dir::cellvars", codeblock)) return 0;
    return 1;
}

/* set-directives = set-version-pyvm + set-flags + set-filename 
+ set-name + set-stack-size + set-arg-count */
static int parse_set_directives(list_t *lexems, py_codeblock *codeblock)
{
    if (!parse_set(lexems, "version_pyvm", codeblock)) return 0;
    if (!parse_set(lexems, "flags", codeblock)) return 0;
    if (!parse_set(lexems, "filename", codeblock)) return 0;
    if (!parse_set(lexems, "name", codeblock)) return 0;
    if (!parse_set(lexems, "stack_size", codeblock)) return 0;
    if (!parse_set(lexems, "arg_count", codeblock)) return 0;
    return 1;
}

/* valable pour traiter set-version-pyvm, set-flags, 
set-filename, set-name, set-stack-size et set-arg-count */
static int parse_set(list_t *lexems, char *set, py_codeblock *codeblock)
{
    if(!next_lexem_is(lexems, "dir::set")) {
        print_parse_error("Erreur: attendu 'dir::set' pour version_pyvm (ligne %d col %d)\n", lexems);
        return 0;
    }
    lexem_advance(lexems);

    if(!next_lexem_is(lexems, set)) {
        print_parse_error("Erreur: nom de directive absent ou incorrect (ligne %d col %d)\n", lexems);
        return 0;
    }
    lexem_advance(lexems);

    if (!strcmp(set, "version_pyvm") 
     || !strcmp(set, "stack_size") 
     || !strcmp(set, "arg_count")) 
    {
        if(!next_lexem_is(lexems, "integer::dec")) {
            print_parse_error("Erreur: attendu 'integer::dec' (ligne %d col %d)\n", lexems);
            return 0;
        }
        {
            int val = atoi(lexem_value(list_first(*lexems)));
            if (!strcmp(set, "version_pyvm")) codeblock->version_pyvm = val;
            else if (!strcmp(set, "stack_size")) codeblock->header.stack_size = val;
            else if (!strcmp(set, "arg_count")) codeblock->header.arg_count = val;
        }
    }
    else if (!strcmp(set, "name") 
          || !strcmp(set, "filename")) 
    {
        if(!next_lexem_is(lexems, "string")) {
        print_parse_error("Erreur: attendu 'string' (ligne %d col %d)\n", lexems);
        return 0;
        }
        {
            lexem_t lx = list_first(*lexems);
            pyobj_t str = new_string_obj(lexem_value(lx), STRING_MARKER);
            if (!strcmp(set, "filename")) codeblock->binary.trailer.filename = str;
            else if (!strcmp(set, "name")) codeblock->binary.trailer.name = str;
        }
    }
    else {
        if(!next_lexem_is(lexems, "integer::hex")) {
            print_parse_error("Erreur: attendu 'integer::hex' (ligne %d col %d)\n", lexems);
            return 0;
        }
        {
            lexem_t lx = list_first(*lexems);
            int val = (int)strtol(lexem_value(lx), NULL, 16); /* Conversion hexadécimale */
            codeblock->header.flags = val;
        }
    }

    lexem_advance(lexems);
    parse_eol_star(lexems);

    return 1;
}

/* interned-strings = {‘dir::interned’} eol ( {‘string’} eol )* */
static int parse_interned_strings(list_t *lexems, py_codeblock *codeblock)
{
    if(!next_lexem_is(lexems, "dir::interned")) {
        print_parse_error("Erreur: attendu 'dir::interned' (ligne %d col %d)\n", lexems);
        return 0;
    }
    lexem_advance(lexems);
    parse_eol_star(lexems);

    pyobj_t interned_node = new_pyobj(SET_MARKER);

    #define MAX_INTERNED 1024
    pyobj_t strings[MAX_INTERNED];
    int count = 0;

    while(next_lexem_is(lexems, "string")) {
        lexem_t lx = list_first(*lexems);
        strings[count] = new_string_obj(lexem_value(lx), STRINGREF_MARKER);
        count++;
        if(count >= MAX_INTERNED) {
            print_parse_error("Erreur: trop de chaînes internées (ligne %d col %d)\n", lexems);
            return 0;
        }
        lexem_advance(lexems);
        parse_eol_star(lexems);
    }

    interned_node->py.list.size = count;
    interned_node->py.list.value = calloc(count, sizeof(pyobj_t));
    for(int i=0; i<count; i++) {
        interned_node->py.list.value[i] = strings[i];
    }

    codeblock->binary.content.interned = interned_node;
    return 1;
}

/* ---- constants = {‘dir::consts’} eol ( constant eol )* ---- */
static int parse_constants(list_t *lexems, py_codeblock *codeblock)
{
    if(!next_lexem_is(lexems, "dir::consts")) {
        print_parse_error("Erreur: attendu 'dir::consts' (ligne %d col %d)\n", lexems);
        return 0;
    }
    lexem_advance(lexems);
    parse_eol_star(lexems);

    #define MAX_CONSTS 1024
    pyobj_t csts[MAX_CONSTS];
    int count = 0;

    while(
       next_lexem_is(lexems, "integer")     /* integer::dec OU integer::hex */
    || next_lexem_is(lexems, "float")
    || next_lexem_is(lexems, "string")
    || next_lexem_is(lexems, "pycst")
    || next_lexem_is(lexems, "paren::left")
 //   || next_lexem_is(lexems, "brack::left")
    || next_lexem_is(lexems, "dir::code_start")
    )
    {
        pyobj_t cst = parse_constant(lexems);
        csts[count++] = cst;
        if(count >= MAX_CONSTS) {
            print_parse_error("Erreur: trop de constantes (ligne %d col %d)\n", lexems);
            return 0;
        }
        parse_eol_star(lexems);
    }

    pyobj_t constants_node = new_pyobj(SET_MARKER);
    constants_node->py.list.size = count;
    constants_node->py.list.value = calloc(count, sizeof(pyobj_t));
    for(int i=0; i<count; i++) {
        constants_node->py.list.value[i] = csts[i];
    }

    codeblock->binary.content.consts = constants_node;
    return 1;
}

/* ---- constant = {‘integer’} | {‘float’} | {‘string’} | {‘pycst’} | tuple ---- */
static pyobj_t parse_constant(list_t *lexems)
{
    /* Au lieu de lexem_peek(...)->type, on utilise next_lexem_is() */
    if(next_lexem_is(lexems, "integer")) {
        /* integer::dec ou integer::hex => on regarde plus précisément */
        lexem_t lx = list_first(*lexems);
        int val;
    
        if(next_lexem_is(lexems, "integer::hex")) val = (int)strtol(lexem_value(lx), NULL, 16);
        else val = (int)strtol(lexem_value(lx), NULL, 10);

        lexem_advance(lexems);
        return new_int_obj(val, INT_MARKER);
    }
    else if(next_lexem_is(lexems, "float")) {
        lexem_t lx = list_first(*lexems);
        double val = atof(lexem_value(lx));
        lexem_advance(lexems);
        return new_float_obj(val);
    }
    else if(next_lexem_is(lexems, "string")) {
        lexem_t lx = list_first(*lexems);
        char* str = lexem_value(lx);
        pyobj_t pobj = new_string_obj(str, STRING_MARKER);
        lexem_advance(lexems);
        return pobj;
    }
    else if(next_lexem_is(lexems, "pycst")) {
        pyobj_type type = 0;
        if (next_lexem_is(lexems, "pycst::None")) type = NONE_MARKER;
        if (next_lexem_is(lexems, "pycst::True")) type = TRUE_MARKER;
        if (next_lexem_is(lexems, "pycst::False")) type = FALSE_MARKER;
        pyobj_t obj = new_pyobj(type);
        lexem_advance(lexems);
        return obj;
    }
    else if(next_lexem_is(lexems, "paren::left") || next_lexem_is(lexems, "brack::left")) {
        return parse_tuple_or_list(lexems);
    }
    else if(next_lexem_is(lexems, "dir::code_start")) {
        return parse_function(lexems);
    }

    print_parse_error("Erreur: constant inconnu (ligne %d col %d)\n", lexems);
    return NULL;
}

/* tuple = {‘paren::left’} ({’blank’} constant )* {‘paren::right’} */
static pyobj_t parse_tuple_or_list(list_t *lexems)
{
    int par = 0;
    if (next_lexem_is(lexems, "paren::left")) par = 1;
    else if (!next_lexem_is(lexems, "brack::left")) {
        print_parse_error("Erreur: attendu '[' ou '(' (ligne %d col %d)\n", lexems);
        return NULL;
    }
    lexem_advance(lexems);

    #define MAX_TUPLE_ELTS 2048
    pyobj_t elts[MAX_TUPLE_ELTS];
    int count = 0;

    while(1) {
        if( next_lexem_is(lexems, "integer")
         || next_lexem_is(lexems, "float")
         || next_lexem_is(lexems, "string")
         || next_lexem_is(lexems, "pycst")
         || next_lexem_is(lexems, "paren::left") )
        {
            elts[count++] = parse_constant(lexems);
            if(count >= MAX_TUPLE_ELTS) {
                print_parse_error("Erreur: tuple trop gros (ligne %d col %d)\n", lexems);
                return NULL;
            }
        }
        else {
            break;
        }
    }

    if(par == 1 && !next_lexem_is(lexems, "paren::right")) {
        print_parse_error("Erreur: attendu ')' (fin de tuple) (ligne %d col %d)\n", lexems);
        return NULL;
    }
    /*else if (!next_lexem_is(lexems, "brack::right")) {
        print_parse_error("Erreur: attendu ']' (fin de tuple) (ligne %d col %d)\n", lexems);
        return NULL;
    }*/
    lexem_advance(lexems);

    pyobj_type type;
    if (par) type = TUPLE_MARKER;
    else type = LIST_MARKER;
    pyobj_t tuple_obj = new_list_obj(type, count);
    for(int i=0; i<count; i++) {
        tuple_obj->py.list.value[i] = elts[i];
    }
    return tuple_obj;
}

/* code = {‘dir::text’} eol ( assembly-line eol )* */
static pyobj_t parse_code(list_t *lexems, py_codeblock *codeblock)
{
    if(!next_lexem_is(lexems, "dir::text")) {
        print_parse_error("Erreur: attendu 'dir::text' pour le code (ligne %d col %d)\n", lexems);
        return NULL;
    }
    lexem_advance(lexems);
    parse_eol_star(lexems);

    char code[100000] = {0};
    char lnt[100000] = {0};

    int count1 = 0;
    int count2 = 0;

    while(
       next_lexem_is(lexems, "insn::0")
    || next_lexem_is(lexems, "insn::1")
    || next_lexem_is(lexems, "dir::line")
    || next_lexem_is(lexems, "symbol")
    )
    {
        parse_eol_star(lexems);
        if(!parse_assembly_line(lexems, code, &count1, lnt, &count2)) return NULL;
        lexem_advance(lexems);
    }

    if (!count1 && !count2) {
        print_parse_error("Erreur: attendu <assembly_line> pour le code (ligne %d col %d)\n", lexems);
        return NULL;
    }
    pyobj_t code_obj = new_string_obj(code, STRING_MARKER);
    codeblock->binary.trailer.lnotab = new_string_obj(lnt, STRING_MARKER);
    return code_obj;
}

/* assembly-line = insn | source-lineno | label */
static int parse_assembly_line(list_t *lexems, char *code, int *count1, char *lnt, int *count2)
{
    /* insn */
    if(next_lexem_is(lexems, "insn::0") || next_lexem_is(lexems, "insn::1")) {
        lexem_t lx_insn = list_first(*lexems);
        int opcode = (int)strtol(lexem_value(lx_insn), NULL, 16);
        char *ptr1 = (char*)&opcode;
        code[*count1] = ptr1[0];
        (*count1)++;
        /* si c’est insn::1 => un argument suit */
        if( lexem_type(lx_insn, "insn::1") ) {
            lexem_advance(lexems);

            if(next_lexem_is(lexems, "integer::dec")) {
                lexem_t lx_arg = list_first(*lexems);
                int arg_val = atoi(lexem_value(lx_arg));
                char *ptr2 = (char*)&arg_val;
                code[*count1] = ptr2[0];
                (*count1)++;
                code[*count1] = ptr2[1];
                (*count1)++;
                // printf( "%d -> %02x %02x %02x %02x\n", arg_val, ptr[0], ptr[1], ptr[2], ptr[3] );
                lexem_advance(lexems);
                return 1;
            }
            else if(next_lexem_is(lexems, "symbol")) {
                //print_parse_error("Erreur: insn::1 attend un entier (dec) mais pas un symbol car on utilise --easy (ligne %d col %d)\n", lexems);
                //return 0;
                return 1;
            }
            else {
                print_parse_error("Erreur: insn::1 attend un entier (dec) (ligne %d col %d)\n", lexems);
                return 0;
            }
        }
        else {
            lexem_advance(lexems);
            return 1;
        }
    }
    /* source-lineno */
    else if(next_lexem_is(lexems, "dir::line")) {
        lexem_advance(lexems);

        if(!next_lexem_is(lexems, "integer::dec")) {
            print_parse_error("Erreur: attendu 'integer::dec' après 'dir::line' (ligne %d col %d)\n", lexems);
            return 0;
        }
        lexem_t lx_num = list_first(*lexems);
        int lineno; 
        if (!(*count2)) lineno = atoi(lexem_value(lx_num));
        else lineno = atoi(lexem_value(lx_num)) - lnt[*count2 - 1];
        char *ptr3 = (char*)&lineno;
        lnt[*count2] = ptr3[0];
        (*count2)++;
        lexem_advance(lexems);
        return 1;
    }
    /* label => symbol blank colon */
    else if(next_lexem_is(lexems, "symbol")) {
        lexem_advance(lexems);

        if(!next_lexem_is(lexems, "colon")) {
            print_parse_error("Erreur: attendu ':' après un 'symbol' (ligne %d col %d)\n", lexems);
            return 0;
        }
        lexem_advance(lexems);
        return 1;
    }
    else {
        print_parse_error("Erreur: assembly-line invalide (ligne %d col %d)\n", lexems);
        return 0;
    }
}

/* function = {‘dir::code_start’} {‘integer’} ⟨eol⟩ ⟨pys⟩ {‘dir::code_end’} */
static pyobj_t parse_function(list_t *lexems) {
    if (!next_lexem_is(lexems, "dir::code_start")) {
        print_parse_error("Erreur: attendu 'dir::code_start' pour la fonction (ligne %d col %d)\n", lexems);
        return NULL;
    }
    lexem_advance(lexems);

    if (!next_lexem_is(lexems, "integer")) {
        print_parse_error("Erreur: attendu 'integer' après 'dir::code_start' (ligne %d col %d)\n", lexems);
        return NULL;
    }

    int func_id = 0;
    {
        lexem_t lx = lexem_advance(lexems);
        func_id = atoi(lexem_value(lx));
    }
    parse_eol_star(lexems);

    pyobj_t func_node = new_pyobj(CODE_MARKER);

    py_codeblock *cb = calloc(1, sizeof(py_codeblock));
    func_node->py.codeblock = cb;
    cb->binary.trailer.firstlineno = func_id;

    if (!parse_set_directives(lexems, cb)) return NULL;

    // 7) [⟨interned-strings⟩] => facultatif lorsque l'on est entre .code_start et .code_end
    if (next_lexem_is(lexems, "dir::interned")) {
        if (!parse_interned_strings(lexems, cb)) return NULL;
    }

    if (!parse_constants(lexems, cb)) return NULL;

    if (!parse_optional(lexems, "dir::names",    cb)) return NULL;
    if (!parse_optional(lexems, "dir::varnames", cb)) return NULL;
    if (!parse_optional(lexems, "dir::freevars", cb)) return NULL;
    if (!parse_optional(lexems, "dir::cellvars", cb)) return NULL;

    {
        pyobj_t code_obj = parse_code(lexems, cb); 
        if (!code_obj) return NULL;
        cb->binary.content.bytecode = code_obj;
    }

    if (!next_lexem_is(lexems, "dir::code_end")) {
        print_parse_error("Erreur: attendu 'dir::code_end' à la fin de la fonction (ligne %d col %d)\n", lexems);
        return NULL;
    }
    lexem_advance(lexems);
    parse_eol_star(lexems);

    return func_node;
}

/* Codes couleurs ANSI (pour console) */
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"

/* Pour rendre l’affichage plus clair, on peut 
   associer une couleur différente à chaque type (ou groupe de types).
   Ici, c’est un choix arbitraire, à ajuster comme vous voulez. */
static const char* color_for_type(char t)
{
    switch(t) {
        case INT_MARKER:            return COLOR_RED;     // i
        case FLOAT_MARKER:          return COLOR_CYAN;    // f
        case STRING_MARKER:         return COLOR_GREEN;   // s
        case STRINGREF_MARKER:      return COLOR_GREEN;   // R
        case TUPLE_MARKER:          return COLOR_YELLOW;  // (
        case LIST_MARKER:           return COLOR_YELLOW;  // [
        case SET_MARKER:            return COLOR_YELLOW;  // <
        case CODE_MARKER:           return COLOR_MAGENTA; // c
        case NONE_MARKER:           return COLOR_WHITE;    // N
        case TRUE_MARKER:           return COLOR_WHITE;    // T
        case FALSE_MARKER:          return COLOR_WHITE;    // F
        default:                    return COLOR_WHITE;    // ?
    }
}

/* Petite fonction pour indenter selon un niveau donné */
static void print_indent(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        printf("  "); // deux espaces par niveau
    }
}

/* Prototype de la fonction principale d’affichage */
static void print_pyobj_rec(pyobj_t obj, int indent_level);

/* Pour afficher le codeblock plus en détail */
static void print_codeblock(py_codeblock *cb, int indent_level)
{
    if (!cb) {
        print_indent(indent_level);
        printf("(codeblock NULL)\n");
        return;
    }

    print_indent(indent_level);
    printf("version_pyvm = %d\n", cb->version_pyvm);

    print_indent(indent_level);
    printf("header.arg_count  = %d\n",  cb->header.arg_count);
    print_indent(indent_level);
    printf("header.local_count = %d\n", cb->header.local_count);
    print_indent(indent_level);
    printf("header.stack_size  = %d\n", cb->header.stack_size);
    print_indent(indent_level);
    printf("header.flags       = 0x%X\n", cb->header.flags);

    /* --- Affichage de la structure binary --- */
    print_indent(indent_level);
    printf("binary.header.magic      = %d\n",   cb->binary.header.magic);
    print_indent(indent_level);
    printf("binary.header.timestamp  = %ld\n",  (long)cb->binary.header.timestamp);
    print_indent(indent_level);
    printf("binary.header.source_size= %d\n",   cb->binary.header.source_size);

    print_indent(indent_level);
    printf("binary.trailer.firstlineno = %d\n", cb->binary.trailer.firstlineno);

    print_indent(indent_level);
    printf("binary.trailer.filename =>\n");
    print_pyobj_rec(cb->binary.trailer.filename, indent_level + 1);

    print_indent(indent_level);
    printf("binary.trailer.name =>\n");
    print_pyobj_rec(cb->binary.trailer.name, indent_level + 1);

    print_indent(indent_level);
    printf("binary.trailer.lnotab =>\n");
    print_pyobj_rec(cb->binary.trailer.lnotab, indent_level + 1);

    /* --- Contenu : interned, consts, bytecode, etc. */
    print_indent(indent_level);
    printf("binary.content.interned =>\n");
    print_pyobj_rec(cb->binary.content.interned, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.consts =>\n");
    print_pyobj_rec(cb->binary.content.consts, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.names =>\n");
    print_pyobj_rec(cb->binary.content.names, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.varnames =>\n");
    print_pyobj_rec(cb->binary.content.varnames, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.freevars =>\n");
    print_pyobj_rec(cb->binary.content.freevars, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.cellvars =>\n");
    print_pyobj_rec(cb->binary.content.cellvars, indent_level + 1);

    print_indent(indent_level);
    printf("binary.content.bytecode =>\n");
    print_pyobj_rec(cb->binary.content.bytecode, indent_level + 1);
}

/**
 * Fonction récursive d’affichage d’un pyobj_t.
 */
static void print_pyobj_rec(pyobj_t obj, int indent_level)
{
    if (!obj) {
        print_indent(indent_level);
        printf("(NULL pyobj)\n");
        return;
    }

    /* Sélection de la couleur (simple) selon le type */
    const char* col = color_for_type(obj->type);

    print_indent(indent_level);
    /* On affiche le type sous forme littérale + la valeur éventuelle */
    switch(obj->type)
    {
        case INT_MARKER:
            printf("%sINT(%d)%s\n", col, obj->py.number.integer, COLOR_RESET);
            break;

        case FLOAT_MARKER:
            printf("%sFLOAT(%g)%s\n", col, obj->py.number.real, COLOR_RESET);
            break;

        case NONE_MARKER:
            printf("%sNone%s\n", col, COLOR_RESET);
            break;

        case TRUE_MARKER:
            printf("%sTrue%s\n", col, COLOR_RESET);
            break;

        case FALSE_MARKER:
            printf("%sFalse%s\n", col, COLOR_RESET);
            break;

        case STRING_MARKER:
            /* On affiche la chaîne */
            printf("%sSTRING(%s)%s\n", col, 
                   (obj->py.string.buffer ? obj->py.string.buffer : ""), 
                   COLOR_RESET);
            break;

        case STRINGREF_MARKER:
            printf("%sSTRINGREF(%s)%s\n", col,
                   (obj->py.string.buffer ? obj->py.string.buffer : ""),
                   COLOR_RESET);
            break;

        case TUPLE_MARKER:
        case LIST_MARKER:
        case SET_MARKER:
        {
            /* TUPLE, LIST et SET sont gérés comme des listes
               dans votre structure. */
            const char *type_name =
                (obj->type == TUPLE_MARKER) ? "TUPLE" :
                (obj->type == LIST_MARKER)  ? "LIST"  : "SET";

            printf("%s%s (size=%d)%s\n", 
                   col, type_name, obj->py.list.size, COLOR_RESET);

            /* Descente récursive sur les éléments du conteneur */
            for(int i = 0; i < obj->py.list.size; i++) {
                print_pyobj_rec(obj->py.list.value[i], indent_level + 1);
            }
        }
        break;

        case CODE_MARKER:
        {
            /* Un objet code : on affiche d’abord un entête simple, 
               puis on détaille la structure du py_codeblock. */
            printf("%sCODE_OBJECT%s\n", col, COLOR_RESET);
            /* On affiche la structure py_codeblock associée */
            print_codeblock(obj->py.codeblock, indent_level + 1);
        }
        break;

        default:
            /* Au besoin, vous pouvez gérer plus de types (INT64_MARKER, 
               COMPLEX_MARKER, etc.). Ici, on en gère un minimal. */
            printf("%sUNKNOWN type (%c)%s\n", col, obj->type, COLOR_RESET);
            break;
    }
}

/**
 * Fonction « publique » appelée depuis l’extérieur pour afficher
 * la racine (renvoyée par parse()). 
 * Par exemple :
 *   pyobj_t root = parse(...);
 *   print_pyobj_tree(root);
 */
void print_pyobj(pyobj_t root)
{
    printf("=== Début de l’affichage de l’arbre pyobj_t ===\n");
    print_pyobj_rec(root, 0);
    printf("=== Fin de l’affichage de l’arbre pyobj_t ===\n");
}

/* Fonction principale de libération */
void free_pyobj(pyobj_t obj) {
    if (!obj) return;
    free_pyobj_rec(obj);
}

/* Fonction récursive interne */
static void free_pyobj_rec(pyobj_t obj) {
    if (!obj) return;

    /* 1) Si obj->py.list.value n'est pas NULL, libérer récursivement chaque enfant */
    if (obj->type == SET_MARKER && obj->py.list.size > 0){
        for (int i = 0; i < obj->py.list.size; i++) {
            free_pyobj_rec(obj->py.list.value[i]);
        }
        free(obj->py.list.value);
        obj->py.list.value = NULL;
        obj->py.list.size = 0;
    }

    /* 2) Si c’est un STRING_NODE ou similaire, libérer la chaîne */

    if (obj->type == STRING_MARKER || obj->type == STRINGREF_MARKER) {
        free(obj->py.string.buffer);
        obj->py.string.buffer = NULL;
        obj->py.string.length = 0;
    }

    /* 3) Si c’est un CODE_MARKER, libérer le codeblock associé */
    if (obj->type == CODE_MARKER && obj->py.codeblock) {
        // Libération des champs internes du py_codeblock
        py_codeblock *cb = obj->py.codeblock;

        /* 3a) libérer les pyobj_t à l’intérieur du codeblock->binary.content */
        if (cb->binary.content.interned) {
            free_pyobj_rec(cb->binary.content.interned);
            cb->binary.content.interned = NULL;
        }
        if (cb->binary.content.bytecode) {
            free_pyobj_rec(cb->binary.content.bytecode);
            cb->binary.content.bytecode = NULL;
        }
        if (cb->binary.content.consts) {
            free_pyobj_rec(cb->binary.content.consts);
            cb->binary.content.consts = NULL;
        }
        if (cb->binary.content.names) {
            free_pyobj_rec(cb->binary.content.names);
            cb->binary.content.names = NULL;
        }
        if (cb->binary.content.varnames) {
            free_pyobj_rec(cb->binary.content.varnames);
            cb->binary.content.varnames = NULL;
        }
        if (cb->binary.content.freevars) {
            free_pyobj_rec(cb->binary.content.freevars);
            cb->binary.content.freevars = NULL;
        }
        if (cb->binary.content.cellvars) {
            free_pyobj_rec(cb->binary.content.cellvars);
            cb->binary.content.cellvars = NULL;
        }

        /* 3b) libérer les pyobj_t du trailer s’ils sont alloués */
        if (cb->binary.trailer.filename) {
            free_pyobj_rec(cb->binary.trailer.filename);
            cb->binary.trailer.filename = NULL;
        }
        if (cb->binary.trailer.name) {
            free_pyobj_rec(cb->binary.trailer.name);
            cb->binary.trailer.name = NULL;
        }
        if (cb->binary.trailer.lnotab) {
            free_pyobj_rec(cb->binary.trailer.lnotab);
            cb->binary.trailer.lnotab = NULL;
        }

        /* 3c) Enfin libérer le codeblock lui-même */
        free(cb);
        obj->py.codeblock = NULL;
    }

    /* 4) Terminer en libérant l’objet lui-même */
    free(obj);
}