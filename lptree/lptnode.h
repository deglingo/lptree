/* lptnode.h -
 */

#ifndef _LPTNODE_H_
#define _LPTNODE_H_

#include "lptree/lptbase.h"
#include "lptree/lptnspec.h"
#include "lptree/lptnode-def.h"



typedef void (* LptForeachFunc) ( LptNode *node,
                                  gpointer data );



/* LptNode:
 */
struct _LptNode
{
  LPT_NODE_INSTANCE_HEADER;

  LObject *key;
  /* [REMOVEME] should go in NData */
  GList *children;
  LObject *value;
};



/* LptNodeClass:
 */
struct _LptNodeClass
{
  LPT_NODE_CLASS_HEADER;
};



LptNode *lpt_node_new ( LptNSpec *nspec );
void lpt_node_set_value ( LptNode *node,
                          LObject *value );
LObject *lpt_node_get_value ( LptNode *node );
void lpt_node_add ( LptNode *node,
                    LptNode *child,
                    LObject *key );
guint lpt_node_get_n_children ( LptNode *node );
LptNode *lpt_node_get_child ( LptNode *node,
                              LObject *key );
void lpt_node_foreach ( LptNode *node,
                        LptForeachFunc func,
                        gpointer data );



#endif
