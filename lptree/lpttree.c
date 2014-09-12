/* lpttree.c -
 */

#include "lptree/lpttree.h"
#include "lptree/lptnspecdir.h"
#include "lptree/lpttree.inl"



/* lpt_tree_new:
 */
LptTree *lpt_tree_new ( void )
{
  LptTree *tree;
  LptNSpec *nspec = lpt_nspec_dir_new("root");
  tree = LPT_TREE(l_object_new(LPT_CLASS_TREE, NULL));
  tree->root = lpt_node_new(nspec, NULL, NULL);
  l_object_unref(nspec);
  return tree;
}



/* lpt_tree_create_node:
 */
LptNode *lpt_tree_create_node ( LptTree *tree,
                                const gchar *path,
                                LptNSpec *nspec )
{
  LptNode *n;
  n = lpt_node_new(nspec, NULL, NULL);
  return n;
}
