#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "m_pd.h"

static t_class* xbsendtiming_class;

typedef struct _node {
  t_atom* list;
  int argc;            // Number of atoms in the list
  struct _node* next;  // Pointer to the next node in the queue
} t_node;

typedef struct _clkll {
  t_clock* clk;
  t_node* head;
  t_node* tail;
} clkll;

typedef struct _xbsendtiming {
  t_object o;
  clkll cmds;
  clkll polls;
  t_outlet* out;
} t_xbsendtiming;

void xbsendtiming_listin(t_xbsendtiming* x, t_symbol* s,  //
                         int argc, t_atom* argv) {
  t_node* new_node = (t_node*)malloc(sizeof(t_node));
  new_node->argc = argc;
  new_node->list = (t_atom*)malloc(argc * sizeof(t_atom));
  memcpy(new_node->list, argv, argc * sizeof(t_atom));
  new_node->next = NULL;

  if (fabs(atom_getfloat(&argv[0]) - 232.0) < 0.1) {
    // This is a Poll.
    if (x->polls.tail == NULL) {
      x->polls.head = new_node;
      x->polls.tail = new_node;
    } else {
      x->cmds.tail->next = new_node;
      x->cmds.tail = new_node;
    }
  } else {
    // This is a non-Poll Command.
    if (x->cmds.tail == NULL) {
      x->cmds.head = new_node;
      x->cmds.tail = new_node;
    } else {
      x->cmds.tail->next = new_node;
      x->cmds.tail = new_node;
    }
  }
}

void cmd_tick(t_xbsendtiming* x) {
  if (x->cmds.head != NULL) {
    t_node* node = x->cmds.head;
    outlet_list(x->out, &s_list, node->argc, node->list);

    // Remove the node from the queue
    x->cmds.head = x->cmds.head->next;
    if (x->cmds.head == NULL) {
      x->cmds.tail = NULL;  // Reset tail if the queue is now empty
    }

    // Free the dequeued node
    free(node->list);
    free(node);
  }

  clock_delay(x->polls.clk, 10);  // Poll next turn.
}

void poll_tick(t_xbsendtiming* x) {
  if (x->polls.head != NULL) {
    t_node* node = x->polls.head;
    outlet_list(x->out, &s_list, node->argc, node->list);

    // Remove the node from the queue
    x->polls.head = x->polls.head->next;
    if (x->polls.head == NULL) {
      x->polls.tail = NULL;  // Reset tail if the queue is now empty
    }

    // Free the dequeued node
    free(node->list);
    free(node);
  }

  clock_delay(x->cmds.clk, 90);  // Command next turn.
}

// Method to initialize the clock and queue when the external is created
void* xbsendtiming_new(void) {
  t_xbsendtiming* x = (t_xbsendtiming*)pd_new(xbsendtiming_class);

  x->cmds.clk = clock_new(x, (t_method)cmd_tick);
  x->cmds.head = NULL;
  x->cmds.tail = NULL;
  x->polls.clk = clock_new(x, (t_method)poll_tick);
  x->polls.head = NULL;
  x->polls.tail = NULL;

  x->out = outlet_new(&x->o, &s_list);

  clock_delay(x->cmds.clk, 1);

  return (void*)x;
}

// Destructor for cleanup
void xbsendtiming_free(t_xbsendtiming* x) {
  clock_free(x->cmds.clk);
  clock_free(x->polls.clk);

  // Free any remaining nodes in the queue
  while (x->cmds.head != NULL) {
    t_node* node = x->cmds.head;
    x->cmds.head = x->cmds.head->next;
    free(node->list);
    free(node);
  }

  // Free any remaining nodes in the queue
  while (x->polls.head != NULL) {
    t_node* node = x->polls.head;
    x->polls.head = x->polls.head->next;
    free(node->list);
    free(node);
  }
}

void xbsendtiming_setup(void) {
  xbsendtiming_class = class_new(
      gensym("xbsendtiming"), (t_newmethod)xbsendtiming_new,
      (t_method)xbsendtiming_free, sizeof(t_xbsendtiming), CLASS_DEFAULT, 0);

  class_addlist(xbsendtiming_class, (t_method)xbsendtiming_listin);
}
