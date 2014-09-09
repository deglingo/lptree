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
