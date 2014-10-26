/* lptnode.c -
 */

#include "lptree/private.h"
#include "lptree/lptnode.h"
#include "lptree/lpttree.h"
#include "lptree/lptnode.inl"

#include <string.h> /* [removeme] */



/* Signals:
 */
enum
  {
    SIG_VALUE_SET = 0,
    SIG_COUNT,
  };

static LSignalID signals[SIG_COUNT] = { 0, };

static volatile guint nid_counter = 1;

static void _dispose ( LObject *object );



/* lpt_node_class_init:
 */
static void lpt_node_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  signals[SIG_VALUE_SET] =
    l_signal_new(cls,
                 "value_set");
}



/* lpt_node_init:
 */
static void lpt_node_init ( LObject *obj )
{
  LPT_NODE(obj)->nid = g_atomic_int_add(&nid_counter, 1);
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



/* lpt_node_get_id:
 */
guint lpt_node_get_id ( LptNode *node )
{
  return node->nid;
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
  _lpt_tree_set_node_value(node->tree, node, value);
}



/* lpt_node_get_value:
 */
LObject *lpt_node_get_value ( LptNode *node )
{
  return l_object_ref(node->value);
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
  ASSERT(key); /* ?? */
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



/* lpt_node_create_child:
 */
LptNode *lpt_node_create_child ( LptNode *node,
                                 LptNSpec *nspec,
                                 LObject *key,
                                 LObject *value )
{
  LptNode *child;
  child = lpt_node_new(nspec);
  if (value)
    lpt_node_set_value(child, value);
  lpt_node_add(node, child, key);
  l_object_unref(child);
  return child;
}



/* lpt_node_get_child:
 */
LptNode *lpt_node_get_child ( LptNode *node,
                              LObject *key )
{
  GList *l;
  ASSERT(key);
  for (l = node->children; l; l = l->next)
    {
      if (l_object_eq(key, LPT_NODE(l->data)->key))
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



/* lpt_node_value_set:
 */
void lpt_node_value_set ( LptNode *node )
{
  l_signal_emit(L_OBJECT(node), signals[SIG_VALUE_SET]);
}
