/* lpttree.h -
 */

#ifndef _LPTTREE_H_
#define _LPTTREE_H_

#include "lptree/lptbase.h"
#include "lptree/lptnode.h"
#include "lptree/lptevent.h"
#include "lptree/lpttree-def.h"



typedef struct _LptClient LptClient;
typedef struct _LptHook LptHook;
typedef gboolean (* LptHookFunc) ( LptEvent *event,
                                   gpointer data );



/* LptHookFlags:
 */
typedef enum _LptHookFlags
  {
    LPT_HOOK_FLAG_DUMMY = 1 << 0,
  }
  LptHookFlags;



/* LptTree:
 */
struct _LptTree
{
  LPT_TREE_INSTANCE_HEADER;

  LptNode *root;
  GList *hooks;
  GList *clients;
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
LptHook *lpt_tree_add_hook ( LptTree *tree,
                             LptHookFunc func,
                             LptHookFlags flags,
                             gpointer data,
                             GDestroyNotify destroy_data );

/* sync */
void lpt_tree_create_share ( LptTree *tree,
                             const gchar *name,
                             const gchar *path,
                             guint flags );
LptClient *lpt_tree_add_client ( LptTree *tree,
                                 const gchar *name );

/* private */
void _lpt_tree_set_node_value ( LptTree *tree,
                                LptNode *node,
                                LObject *value );



#endif
