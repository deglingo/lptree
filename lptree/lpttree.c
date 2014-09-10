/* lpttree.c -
 */

#include "lptree/lpttree.h"
#include "lptree/lpttree.inl"



/* lpt_tree_new:
 */
LptTree *lpt_tree_new ( void )
{
  LptTree *tree;
  tree = LPT_TREE(l_object_new(LPT_CLASS_TREE, NULL));
  return tree;
}
