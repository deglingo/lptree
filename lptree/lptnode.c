/* lptnode.c -
 */

#include "lptree/private.h"
#include "lptree/lptnode.h"
#include "lptree/lpttree.h"
#include "lptree/lptnode.inl"

#include <string.h> /* [removeme] */



static void _dispose ( LObject *object );



/* lpt_node_class_init:
 */
static void lpt_node_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* lpt_node_new:
 */
LptNode *lpt_node_new ( LptNSpec *nspec )
{
  ASSERT(LPT_NSPEC_GET_CLASS(nspec)->create_node);
  return LPT_NSPEC_GET_CLASS(nspec)->create_node(nspec);
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LptNode *node = LPT_NODE(object);
  L_OBJECT_CLEAR(node->nspec);
  L_OBJECT_CLEAR(node->key);
  L_OBJECT_CLEAR(node->value);
  g_list_free_full(node->children, l_object_unref);
  node->children = NULL;
  ((LObjectClass *) parent_class)->dispose(object);
}



/* lpt_node_get_tree:
 */
LptTree *lpt_node_get_tree ( LptNode *node )
{
  return node->tree;
}



/* lpt_node_get_nspec:
 */
LptNSpec *lpt_node_get_nspec ( LptNode *node )
{
  return node->nspec;
}



/* lpt_node_set_value:
 */
void lpt_node_set_value ( LptNode *node,
                          LObject *value )
{
  l_object_ref(value);
  if (node->value)
    L_OBJECT_CLEAR(node->value);
  node->value = value;
}



/* lpt_node_get_value:
 */
LObject *lpt_node_get_value ( LptNode *node )
{
  return node->value;
}



static void _fix_tree ( LptNode *node,
                        LptTree *tree )
{
  ASSERT(!node->tree);
  node->tree = tree;
  lpt_node_foreach(node, (LptForeachFunc) _fix_tree, tree);
}



/* lpt_node_add:
 */
void lpt_node_add ( LptNode *node,
                    LptNode *child,
                    LObject *key )
{
  ASSERT(!child->tree);
  ASSERT(L_IS_STRING(key));
  if (node->tree)
    _fix_tree(child, node->tree);
  child->key = l_object_ref(key);
  node->children = g_list_append(node->children,
                                 l_object_ref(child));
}



/* lpt_node_get_n_children:
 */
guint lpt_node_get_n_children ( LptNode *node )
{
  return g_list_length(node->children);
}



/* lpt_node_get_child:
 */
LptNode *lpt_node_get_child ( LptNode *node,
                              LObject *key )
{
  GList *l;
  ASSERT(L_IS_STRING(key));
  for (l = node->children; l; l = l->next)
    {
      if (!strcmp(L_STRING(key)->str, L_STRING(LPT_NODE(l->data)->key)->str))
        return LPT_NODE(l->data);
    }
  return NULL;
}



/* lpt_node_foreach:
 */
void lpt_node_foreach ( LptNode *node,
                        LptForeachFunc func,
                        gpointer data )
{
  GList *l;
  for (l = node->children; l; l = l->next)
    {
      func(LPT_NODE(l->data), data);
    }
}
