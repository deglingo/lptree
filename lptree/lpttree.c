/* lpttree.c -
 */

#include "lptree/lpttree.h"
#include "lptree/lptnspecdir.h"
#include "lptree/lpttree.inl"



static void _dispose ( LObject *object );



/* lpt_tree_class_init:
 */
static void lpt_tree_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* lpt_tree_new:
 */
LptTree *lpt_tree_new ( void )
{
  LptTree *tree;
  LptNSpec *nspec = lpt_nspec_dir_new("root");
  tree = LPT_TREE(l_object_new(LPT_CLASS_TREE, NULL));
  tree->root = lpt_node_new(nspec);
  l_object_unref(nspec);
  return tree;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(LPT_TREE(object)->root);
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



/* lpt_tree_create_node:
 */
LptNode *lpt_tree_create_node ( LptTree *tree,
                                const gchar *path,
                                LptNSpec *nspec )
{
  LptNode *n;
  n = lpt_node_new(nspec);
  return n;
}
