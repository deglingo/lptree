/* lpttree.c -
 */

#include "lptree/private.h"
#include "lptree/lpttree.h"
#include "lptree/lptnspecdir.h"
#include "lptree/lptpath.h"
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
  const gchar *p = path;
  LObject *key, *next_key;
  LptNode *parent, *n;
  /* find the parent */
  parent = tree->root;
  key = lpt_path_next_key(&p);
  ASSERT(key);
  next_key = lpt_path_next_key(&p);
  while (next_key) {
    parent = lpt_node_get_child(parent, key);
    ASSERT(parent);
    l_object_unref(key);
    key = next_key;
    next_key = lpt_path_next_key(&p);
  }
  /* create the node */
  n = lpt_node_new(nspec);
  /* add it to parent */
  lpt_node_add(parent, n, key);
  l_object_unref(key);
  l_object_unref(n);
  return n;
}



/* lpt_tree_get_node:
 */
LptNode *lpt_tree_get_node ( LptTree *tree,
                             const gchar *path )
{
  LptNode *node = tree->root;
  const gchar *p = path;
  LObject *key;
  while ((key = lpt_path_next_key(&p))) {
    node = lpt_node_get_child(node, key);
    l_object_unref(key);
    if (!node)
      return NULL;
  }
  return node;
}
