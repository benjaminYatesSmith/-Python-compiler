/**
 * @file queue.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief Queue.
 *
 * Queue.
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pyas/list.h> /* list_t for queue_to_list */

  typedef struct link_t *queue_t;

  queue_t queue_new( void );
  int     queue_empty( queue_t q );
  queue_t enqueue( queue_t q, void* object );
  list_t  queue_to_list( queue_t q );

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_H_ */
