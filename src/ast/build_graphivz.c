#include <stdio.h>

static void add_label_print(const char *label, const char *color,
                            const char *shape) {

  if (label || color || shape) {
    printf(" [ ");
    if (label) {
      printf("label = \"%s\"", label);
      if (color) {
        printf(", ");
      }
    }
    if (color) {
      printf("color = %s", color);
      if (shape) {
        printf(", ");
      }
    }
    if (shape) {
      printf("shape = %s", shape);
    }
    printf(" ]");
  }
}

// create a node
void create_node(int node, const char *label, const char *color,
                 const char *shape) {
  printf("\t%d ", node);
  add_label_print(label, color, shape);
  printf(";\n");
}

// link two ast with their node
void link(int node1, int node2, const char *label, const char *color) {
  printf("\t%d -> %d", node1, node2);
  add_label_print(label, color, NULL);
  printf(";\n");
}
