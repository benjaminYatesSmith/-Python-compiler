/**
 * @file list.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief Lists.
 *
 * Lists.
 */

#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* size_t */

  /*
    Make `list_t` an opaque type. A `list_t` is a pointer type
    to some structure that is only defined in `src/list.c`. The
    user has no knowledge of the internals of a `struct link_t`
    and therefore s/he is forced to use only the following
    interface to manipulate lists:
   */



  typedef struct link_t *list_t;

  /*
    We will be using generic lists. See `src/list.c` for details
    on `void*` generic pointers and `action_t` callbacks.
   */
#include <pyas/callbacks.h>

  list_t list_new( void );
  int    list_is_empty( list_t l );
  void*  list_first( list_t l );
  list_t list_next( list_t l );
  list_t list_add_first( void *object, list_t l );
  list_t list_add_last( list_t l, void* object );
  size_t list_length( list_t l );
  list_t list_del_first( list_t l, action_t delete );
  void   list_delete( list_t l, action_t delete );
  int    list_print( list_t l, action_t print );

  /*
    Lists are the fundamental data structure in the LISP family of
    language (Scheme, etc.) These languages have been so influential,
    we pay tribute to them:
  */
#define cons( object, list ) list_add_first( (object), (list) )
#define car( list )          list_first( list )
#define cdr( list )          list_next( list )
#define nilp( list )         list_is_empty( list )
#define nil                  list_new()

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H_ */
