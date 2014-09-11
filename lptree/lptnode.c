/* lptnode.c -
 */

#include "lptree/lptnode.h"
#include "lptree/lptnode.inl"



/* lpt_node_new:
 */
LptNode *lpt_node_new ( LptNSpec *nspec,
                        LptNode *parent,
                        LObject *key )
{
  LptNode *node;
  node = LPT_NODE(l_object_new(LPT_CLASS_NODE, NULL));
  return node;
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
