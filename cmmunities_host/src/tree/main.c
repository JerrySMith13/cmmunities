#include "tree.h"


int main(void){
  Tree test;
  int tree_success = open_tree("new_tree.tree", &test);
  if (tree_success == 0) {
    printf("Need to handle some errors son");
    return -1;
  }
  close_tree(&test);
  
  return read_tree_direct("new_tree.tree");
}
