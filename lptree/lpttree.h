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



#endif
