/* lptnode.h -
 */

#ifndef _LPTNODE_H_
#define _LPTNODE_H_

#include "lptree/lptbase.h"
#include "lptree/lptnspec.h"
#include "lptree/lptnode-def.h"



/* LptNode:
 */
struct _LptNode
{
  LPT_NODE_INSTANCE_HEADER;
};



/* LptNodeClass:
 */
struct _LptNodeClass
{
  LPT_NODE_CLASS_HEADER;
};



LptNode *lpt_node_new ( LptNSpec *nspec,
                        LptNode *parent,
                        LObject *key );
void lpt_node_set_value ( LptNode *node,
                          LObject *value );



#endif
