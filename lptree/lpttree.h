/* lpttree.h -
 */

#ifndef _LPTTREE_H_
#define _LPTTREE_H_

#include "lptree/lptbase.h"
#include "lptree/lptnode.h"
#include "lptree/lpttree-def.h"



/* LptTree:
 */
struct _LptTree
{
  LPT_TREE_INSTANCE_HEADER;

  LptNode *root;
};



/* LptTreeClass:
 */
struct _LptTreeClass
{
  LPT_TREE_CLASS_HEADER;
};



LptTree *lpt_tree_new ( void );
LptNode *lpt_tree_create_node ( LptTree *tree,
                                const gchar *path,
                                LptNSpec *nspec );
LptNode *lpt_tree_get_node ( LptTree *tree,
                             const gchar *path );



#endif
