/**
 * @file queue.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @brief Queue.
 *
 * Queue.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <pyas/queue.h>
#include <pyas/list.h>

struct link_t {
  struct link_t *next;
  void          *content;
};

queue_t queue_new( void ) {
  return NULL;
}

int queue_empty( queue_t q ) {
  return q == NULL;
}

queue_t enqueue(queue_t q, void* object) {
    struct link_t *new = malloc(sizeof(*new));
    if (new == NULL) {
        fprintf(stderr, "Memory allocation failed dans la fonction enqueue\n");
        exit(EXIT_FAILURE);
    }

    new->content = object;

    if(queue_empty(q)) {
        new->next = new; // Pointage circulaire
        return new;
    }

    new->next = q->next; 
    q->next = new;      
    q = new;         

    return q;
}


void* dequeue(queue_t *q) {
    if (queue_empty(*q))
        return NULL;

    // La tête de la file est q->next
    struct link_t *head = (*q)->next;
    void *content = head->content;

    if (head == *q) {
        // Il n'y a qu'un seul élément dans la file
        *q = NULL;
    } else {
        // Mettre à jour le pointeur du dernier élément pour qu'il pointe au nouveau premier
        (*q)->next = head->next;
    }

    free(head); //FIFO
    return content;
}

list_t queue_to_list( queue_t q ) {
  list_t l = list_new();
    if (!q) {
      // La file est vide => retourne une liste vide
      return list_new();
  }

  l = (list_t)q->next;
  q->next = NULL;

  return l;
}



