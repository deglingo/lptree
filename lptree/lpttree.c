/* lpttree.c -
 */

#include "lptree/lpttree.h"



/* lpt_tree_new:
 */
LptTree *lpt_tree_new ( void )
{
  LptTree *tree;
  tree = malloc(sizeof(LptTree));
  return tree;
}
