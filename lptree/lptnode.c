/* lptnode.c -
 */

#include "lptree/lptnode.h"
#include "lptree/lptnode.inl"



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
  LptNode *node;
  node = LPT_NODE(l_object_new(LPT_CLASS_NODE, NULL));
  return node;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  g_list_free_full(LPT_NODE(object)->children, l_object_unref);
  LPT_NODE(object)->children = NULL;
  ((LObjectClass *) parent_class)->dispose(object);
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



/* lpt_node_add:
 */
void lpt_node_add ( LptNode *node,
                    LptNode *child,
                    LObject *key )
{
  node->children = g_list_append(node->children,
                                 l_object_ref(child));
}



/* lpt_node_get_child:
 */
LptNode *lpt_node_get_child ( LptNode *node,
                              LObject *key )
{
  if (node->children)
    return LPT_NODE(node->children->data);
  else
    return NULL;
}
