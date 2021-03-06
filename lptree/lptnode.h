/* lptnode.h -
 */

#ifndef _LPTNODE_H_
#define _LPTNODE_H_

#include "lptree/lptbase.h"
#include "lptree/lptnspec.h"
#include "lptree/lptnode-def.h"

struct _LptTree;



typedef void (* LptForeachFunc) ( LptNode *node,
                                  gpointer data );



/* LptNode:
 */
struct _LptNode
{
  LPT_NODE_INSTANCE_HEADER;

  guint nid;
  struct _LptTree *tree;
  LptNSpec *nspec;
  LObject *key;
  /* [REMOVEME] should go in NData */
  GList *children;
  LObject *value;
  gpointer share; /* set by LptTree */
};



/* LptNodeClass:
 */
struct _LptNodeClass
{
  LPT_NODE_CLASS_HEADER;
};



LptNode *lpt_node_new ( LptNSpec *nspec );
guint lpt_node_get_id ( LptNode *node );
struct _LptTree *lpt_node_get_tree ( LptNode *node );
LptNSpec *lpt_node_get_nspec ( LptNode *node );
void lpt_node_set_value ( LptNode *node,
                          LObject *value );
LObject *lpt_node_get_value ( LptNode *node );
void lpt_node_add ( LptNode *node,
                    LptNode *child,
                    LObject *key );
LptNode *lpt_node_create_child ( LptNode *node,
                                 LptNSpec *nspec,
                                 LObject *key,
                                 LObject *value );
guint lpt_node_get_n_children ( LptNode *node );
LptNode *lpt_node_get_child ( LptNode *node,
                              LObject *key );
void lpt_node_foreach ( LptNode *node,
                        LptForeachFunc func,
                        gpointer data );
void lpt_node_value_set ( LptNode *node );



#endif
