/**
 * @file lexem.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief Lexems.
 *
 * Lexems.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


#include <pyas/lexem.h>
#include <pyas/list.h>
#include <pyas/queue.h> 
#include <pyas/regexp.h>
#include <pyas/re_match.h>
#include <pyas/chargroup.h>
#include <pyas/regexp.h>

struct lexem {
  char *type;
  char *value;
  int   line;    /* Start at line 1   */
  int   column;  /* Start at column 0 */
};

struct lexdef{
  char *type; //par exemple "keyword" ou "identifier"
  char *regexp_str; //l'expression reguliere
  list_t regexp_list; //liste renvoyé par reread
};

/*
  Constructor and callbacks for lists/queues of lexems:
 */

//Cette fonction permet de créer un lexem_t à partir du type, de la valeur, de la line et de la colonne données.
lexem_t lexem_new( char *type, char *value, int line, int column ) {
  //On alloue de la mémoire pour un lexem_t
  lexem_t lex = calloc( 1, sizeof( *lex ) );
  //On vérifie si lex est différent de NULL
  assert( lex );

  //On vérifie si type et value et leurs pointeurs respectifs sont différents de NULL, si oui, on les copie avec la fonction strdup
  //Dans le lexem courant
  if ( type  && *type  ) lex->type  = strdup( type );
  if ( value && *value ) lex->value = strdup( value );

  lex->line   = line;
  lex->column = column;

  return lex;
}

static int re_node_delete_cb(void *data) {
    free(data);
    return 0;
}

static void re_delete(list_t re_list)
{
    if (!re_list) return;
    list_delete(re_list, re_node_delete_cb);
}

//Callback pour détruire un lexdef_t
static int lexdef_delete_cb(void *_ld)
{
    lexdef_t ld = (lexdef_t)_ld;
    if (ld) {
        free(ld->type);
        free(ld->regexp_str);

        /* Libération de la liste retournée par re_read(...) */
        re_delete(ld->regexp_list);
        ld->regexp_list = NULL;
    }
    free(ld);
    return 0;
}

// permet de libérer le fichier de def des lexems
static void free_lexdef_list(list_t def_list)
{
    list_delete(def_list, lexdef_delete_cb);
}




// Cette fonction permet d'afficher un lexem courant dans le fichier assembleur à analyser.
int lexem_print( void *_lex ) {
  lexem_t lex = _lex;
  return printf( "[%d:%d:%s] %s",
         lex->line,
         lex->column,
         lex->type,
         lex->value );
}

int lexdef_print( void *_lexdef) {
  lexdef_t lexdef = _lexdef; 

  printf("%s\n%s\n", lexdef->type, lexdef->regexp_str);
  re_print(lexdef->regexp_list);
  printf("\n\n");
  return 0;
}

int lexem_delete( void *_lex ) {
  lexem_t lex = _lex;

  if ( lex ) {
    free( lex->type );
    free( lex->value );
  }

  free( lex );

  return 0;
}

//------------------------------------------------------------------------------------


/*à partir du chemin d'accès du fichier contenant les définitions de lexèmes et du fichier assembleur à analyser, 
cette fonction renvoie une liste de lexdef_t.*/
list_t lex(char *regexp_file, char *source_file) {
  /*Lire les définitions de lexèmes de la table des lexems */
  list_t regexp_definitions = list_of_defintions(regexp_file);
  if (!regexp_definitions) {
    fprintf(stderr, "Erreur: Aucune définition de lexèmes chargée depuis '%s'.\n", regexp_file);
    return NULL;
  }

    
  list_t def_list = regexp_definitions;
    

  /* Lire le code source assembleur en une seule chaîne, celle qui est contenue à l'interieur du fichier texte du code source assembleur*/
  char *source_code = file_to_string(source_file);
  //On Vérifie si le fichier existe
  if (!source_code) {
    fprintf(stderr, "Erreur: le fichier source '%s' n'existe pas.\n", source_file);
    free_lexdef_list(def_list);
    return NULL;
  }

  // Vérifier si le fichier est vide (chaine vide)
  if (source_code[0] == '\0') {
    fprintf(stderr, "Erreur: le fichier source '%s' est vide.\n", source_file);
    free(source_code);
    free_lexdef_list(regexp_definitions);
    return NULL;
  }

  list_t lexems_list = list_new();  
  queue_t lexems_queue = queue_new();
  int line = 1; 
  int column = 0;

    /*Parcourir la chaîne source jusqu'à la fin */
    //ON parcours par pointeur caractère par caractère
  char *current = source_code; 
  while (*current != '\0') {
      /* On essaye de matcher une expression régulière définie. */
    int matched = 0;
    lexdef_t found_def = NULL;
    char *best_end = NULL;  /* Pointeur fin de match pour la definition courante */
    size_t best_len = 0;    /* Longueur max qu'on peut matcher*/
        
        /* On itère sur la liste des définitions (lexdef_t) pour trouver la première correspondance*/
    list_t tmp = def_list;
    while (!list_is_empty(tmp)) {
      lexdef_t def = (lexdef_t)list_first(tmp);
      tmp = list_next(tmp);
      char *end_ptr = current;
      int ok = re_match(def->regexp_list, current, &end_ptr);
      if (ok) {
        size_t length_matched = (size_t)(end_ptr - current);
        found_def = def;
        best_len  = length_matched;
        best_end  = end_ptr;
        matched   = 1;
        break; // On arrête la boucle sur def_list, car on a trouvé un match
        }
      }

    if (!matched) {
      /* Si on n'a trouvé aucune expression régulière pour la portion courante,
          c'est une erreur de syntaxe. */
      fprintf(stderr, "Erreur lexicale: Aucun lexème ne correspond à la position courante (line %d, colonne %d).\n",line, column);
      free(source_code);
      lexems_list = queue_to_list(lexems_queue);
      list_delete(lexems_list, lexem_delete);
      free_lexdef_list(def_list);
      return NULL;
    }
        
    /*On a un match => Créer un lexem_t pour la portion matched. */
    size_t length_matched = best_len;
    char *lex_value = strndup(current, length_matched);
    if (!lex_value) {
      fprintf(stderr, "Erreur d'allocation mémoire pour la valeur du lexème.\n");
      free(source_code);
      lexems_list = queue_to_list(lexems_queue);
      list_delete(lexems_list, lexem_delete);
      free_lexdef_list(def_list);
      return NULL;
    }

        /* Créer le lexem avec lexem_new(type, value, line, column). 
           found_def->type est le type de lexème, ex: "keyword", "identifier" */
    lexem_t lex = lexem_new(found_def->type, lex_value, line, column);
    free(lex_value); // on peut free car lexem_new a fait un strdup
        
      /*L'ajouter à la liste de lexèmes. */
    lexems_queue = enqueue(lexems_queue,lex);
    //lexems = list_add_last(lexems, lex);
    
    

        /*Mettre à jour la position (line, column) en fonction des caractères consommés. */
    for (size_t i = 0; i < length_matched; i++) {
      if (current[i] == '\n') {
        line++;
        column = 0;
      } else {
          column++;
      }
    }
        /*Avancer dans la chaîne source. */
    current = best_end;
  }

  free(source_code);
  free_lexdef_list(def_list);

  lexems_list = queue_to_list(lexems_queue);

  return lexems_list; 
}

/*----------------------------------------------------------*/
list_t list_of_defintions(char *regexp_file){

//Cette fonction permet de lire n'importe quel fichier texte qui n'est pas nul sous la forme d'une liste de lexdef_t
//Cette fonction est adaptée pour les fichier respectant la grammaire EBNF.
  FILE *fic_regexp = fopen(regexp_file, "r");
  if(fic_regexp == NULL){
    fprintf(stderr, "Erreur lors de l'ouverture du fichier regexp");
    return NULL;
  }

  queue_t definitions = queue_new();

  char line[512]; //taille suffisante
  int line_number = 0;

  while(fgets(line, sizeof(line), fic_regexp) != NULL){
    /*on chercher le '\n afin de le supprimer
    ->permet de ne pas interférer si on fait des recherches
    ->permet de conserver seulement l'information utile*/

    line_number++;
    char * p = strchr(line,'\n');
    if(p) //on vérifie qu'on pointe vers une adresse non NULL
      *p = '\0'; //on élimine le caractère 

    /*on ignore maintenant les espaces*/
    p = line;
    while(isspace((unsigned char)*p))
      p++; // on avance tant qu'on est dans les espaces

    if(*p == '\0') //une line totalement vide
      continue; //permet de retourner en haut de la boucle
    if(*p == '#') //on ignore les lines commençant par '#'
      continue;

    char lextype[128] = {0}; //Facon idiomatique d'initialiser un tableau a 0
    int n = 0; //permet de savoir la position actuelle 

    if(sscanf(p, "%127s%n", lextype, &n) != 1){
      fprintf(stderr, "Warning: parsing type de lexème manquant a la line %d.\n", line_number);
      continue;
    }

    p = p+n; //avance le pointeur

    while(isspace((unsigned char)*p))
      p++;


    if(*p == '\0'){
      fprintf(stderr, "Warning: Expression regulière manquante a la line %d.\n", line_number );
      continue; //il n'y a plus rien (pas d'expression reg donc on saute)

    }

    /*Il ne reste plus que l'expression regulière*/
    char *regexp_str = strdup(p);
    if(!regexp_str){
      fprintf(stderr, "Erreur: problemme d'allocation memoire dans la fonction 'list_of_definition a la line %d' \n", line_number);
      fclose(fic_regexp);
      return NULL;
    }

    list_t parsed_regexp = re_read(regexp_str);

    lexdef_t lexdef = malloc(sizeof(*lexdef));
    if (!lexdef) {
      fprintf(stderr, "Erreur d'allocation mémoire pour la définition a la line %d.\n", line_number);
      free(regexp_str);
      fclose(fic_regexp);
      return NULL;
    }

    lexdef->type = strdup(lextype);
    lexdef->regexp_str = regexp_str;
    lexdef->regexp_list = parsed_regexp;

    definitions = enqueue(definitions, lexdef);
  }

  fclose(fic_regexp);
  return queue_to_list(definitions);
}

/*------------------------------------------------------*/
//permet de convertir un fichier (texte dans notre cas) en la chaine de caractère qu'il contient
char *file_to_string(char *source_file){
  //fopen avec "r" : Ouvre un fichier pour lecture. Si le fichier n'existe pas, l'appel échoue.
  //fopen renvoie un pointeur vers un objet FILE qui est utilisé par d'autres fonctions pour effectuer des opérations sur le fichier
  FILE *fic_regexp = fopen(source_file, "r");
  if(!fic_regexp){
    fprintf(stderr, "Erreur d'ouverture du fichier dans la fonction file_to_string\n");
    return NULL; 
  }

  fseek(fic_regexp, 0, SEEK_END); //on depalce le curseur vers la fin du fichier, pour calculer la taille
  long file_size = ftell(fic_regexp); //on obtient la taille(car le curseur est à la fin)
  rewind(fic_regexp); //on remet le curseur au début du fichier

  char *buffer = malloc(file_size + 1); //+1 pour le '\0'
  if (!buffer) {
    fprintf(stderr, "Erreur d'allocation mémoire pour le buffer du fichier source.\n");
    fclose(fic_regexp);
    return NULL;
  }

  size_t read_size = fread(buffer, 1, file_size, fic_regexp); //permet de savoir la taille en octet
  
  buffer[read_size] = '\0'; 
  /*on force le '/0' afin de pouvoir traiter notre allocation dynamique comme une 
  chaine de caractères*/

  fclose(fic_regexp);
  return buffer;
}

lexem_t lexem_peek( list_t *lexems ) {
  list_t l = *lexems;
  while (lexem_type(list_first(l), "comment") || lexem_type(list_first(l), "blank"))
    l = list_next(l);
  return list_first(l);
}

lexem_t lexem_advance( list_t *lexems ) {
  do {
    *lexems = list_del_first(*lexems, lexem_delete);// on supprime les commentaires et les blancs
  } while (lexem_type(list_first(*lexems), "comment") || lexem_type(list_first(*lexems), "blank"));
  return list_first(*lexems);
}

int next_lexem_is( list_t *lexems, char *type ) {
  return lexem_type(lexem_peek(lexems), type); 
}

int lexem_type_strict( lexem_t lex, char *type ) {
  return !strcmp( lex->type, type );
}

int lexem_type( lexem_t lex, char *type ) {
    return lex->type == strstr( lex->type, type );
}

int value_compare( lexem_t lex, char* value ) {
  return !strcmp(lex->value, value);
}

char *lexem_value( lexem_t lexem ) {
  return lexem->value;
}

int lexem_line( lexem_t lex ) {
  assert(lex);
  return lex->line;
} 

int lexem_col( lexem_t lex ) {
  assert(lex);
  return lex->column;
}
