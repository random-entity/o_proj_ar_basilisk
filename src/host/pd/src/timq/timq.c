#include <stdlib.h>
#include <string.h>

#include "m_pd.h"

static t_class* timq_class;

// Structure to hold each list as a node in the queue
typedef struct _node {
  t_atom* list;
  int argc;            // Number of atoms in the list
  struct _node* next;  // Pointer to the next node in the queue
} t_node;

typedef struct _timq {
  t_object o;
  t_clock* clk;
  t_node* head;
  t_node* tail;
  t_outlet* out;
  // int temp;
} t_timq;

void timq_listin(t_timq* x, t_symbol* s, int argc, t_atom* argv) {
  t_node* new_node = (t_node*)malloc(sizeof(t_node));
  new_node->argc = argc;
  new_node->list = (t_atom*)malloc(argc * sizeof(t_atom));
  memcpy(new_node->list, argv, argc * sizeof(t_atom));
  new_node->next = NULL;

  if (x->tail == NULL) {
    x->head = new_node;
    x->tail = new_node;
  } else {
    x->tail->next = new_node;
    x->tail = new_node;
  }
}

// Function to dequeue and send the list at the appropriate time
void timq_tick(t_timq* x) {
  if (x->head != NULL) {
    // Output the first list in the queue
    t_node* node = x->head;
    outlet_list(x->out, &s_list, node->argc, node->list);

    // Remove the node from the queue
    x->head = x->head->next;
    if (x->head == NULL) {
      x->tail = NULL;  // Reset tail if the queue is now empty
    }

    // Free the dequeued node
    free(node->list);
    free(node);
  }

  clock_delay(x->clk, 100);
}

// Method to initialize the clock and queue when the external is created
void* timq_new(void) {
  t_timq* x = (t_timq*)pd_new(timq_class);

  x->head = NULL;
  x->tail = NULL;

  x->out = outlet_new(&x->o, &s_list);

  x->clk = clock_new(x, (t_method)timq_tick);
  clock_delay(x->clk, 100);

  return (void*)x;
}

// Destructor for cleanup
void timq_free(t_timq* x) {
  clock_free(x->clk);

  // Free any remaining nodes in the queue
  while (x->head != NULL) {
    t_node* node = x->head;
    x->head = x->head->next;
    free(node->list);
    free(node);
  }
}

void timq_setup(void) {
  timq_class = class_new(gensym("timq"), (t_newmethod)timq_new,
                         (t_method)timq_free, sizeof(t_timq), CLASS_DEFAULT, 0);

  class_addlist(timq_class, (t_method)timq_listin);
}
