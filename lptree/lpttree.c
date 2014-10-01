/* lpttree.c -
 */

#include "lptree/private.h"
#include "lptree/lpttree.h"
#include "lptree/lptnspecdir.h"
#include "lptree/lptpath.h"
#include "lptree/lpttree.inl"



/* LptClient:
 */
struct _LptClient
{
  LptTree *tree;
  gchar *name;
};



/* LptHook:
 */
struct _LptHook
{
  LptHookFunc func;
  gpointer data;
};



static void _dispose ( LObject *object );



/* lpt_client_new:
 */
static LptClient *lpt_client_new ( LptTree *tree,
                                   const gchar *name )
{
  LptClient *client = g_new0(LptClient, 1);
  client->tree = tree;
  client->name = g_strdup(name);
  return client;
}



/* lpt_client_free:
 */
static void lpt_client_free ( LptClient *client )
{
  g_free(client->name);
  g_free(client);
}



/* lpt_hook_new:
 */
static LptHook *lpt_hook_new ( void )
{
  return g_new0(LptHook, 1);
}



/* lpt_hook_free:
 */
static void lpt_hook_free ( LptHook *hook )
{
  g_free(hook);
}



/* lpt_tree_class_init:
 */
static void lpt_tree_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* lpt_tree_new:
 */
LptTree *lpt_tree_new ( void )
{
  LptTree *tree;
  LptNSpec *nspec = lpt_nspec_dir_new("root");
  tree = LPT_TREE(l_object_new(LPT_CLASS_TREE, NULL));
  tree->root = lpt_node_new(nspec);
  /* [fixme] ?? */
  tree->root->tree = tree;
  l_object_unref(nspec);
  return tree;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LptTree *tree = LPT_TREE(object);
  L_OBJECT_CLEAR(tree->root);
  g_list_free_full(tree->hooks, (GDestroyNotify) lpt_hook_free);
  tree->hooks = NULL;
  g_list_free_full(tree->clients, (GDestroyNotify) lpt_client_free);
  tree->clients = NULL;
  if (tree->destroy_handler_data) {
    tree->destroy_handler_data(tree->handler_data);
    tree->destroy_handler_data = NULL;
  }
  tree->handler_data = NULL;
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



/* lpt_tree_create_node:
 */
LptNode *lpt_tree_create_node ( LptTree *tree,
                                const gchar *path,
                                LptNSpec *nspec )
{
  const gchar *p = path;
  LObject *key, *next_key;
  LptNode *parent, *n;
  /* find the parent */
  parent = tree->root;
  key = lpt_path_next_key(&p);
  ASSERT(key);
  next_key = lpt_path_next_key(&p);
  while (next_key) {
    parent = lpt_node_get_child(parent, key);
    ASSERT(parent);
    l_object_unref(key);
    key = next_key;
    next_key = lpt_path_next_key(&p);
  }
  /* create the node */
  n = lpt_node_new(nspec);
  /* add it to parent */
  lpt_node_add(parent, n, key);
  l_object_unref(key);
  l_object_unref(n);
  return n;
}



/* lpt_tree_get_node:
 */
LptNode *lpt_tree_get_node ( LptTree *tree,
                             const gchar *path )
{
  LptNode *node = tree->root;
  const gchar *p = path;
  LObject *key;
  while ((key = lpt_path_next_key(&p))) {
    node = lpt_node_get_child(node, key);
    l_object_unref(key);
    if (!node)
      return NULL;
  }
  return node;
}



/* lpt_tree_add_hook:
 */
LptHook *lpt_tree_add_hook ( LptTree *tree,
                             LptHookFunc func,
                             LptHookFlags flags,
                             gpointer data,
                             GDestroyNotify destroy_data )
{
  LptHook *hook = lpt_hook_new();
  hook->func = func;
  hook->data = data;
  tree->hooks = g_list_append(tree->hooks, hook);
  return hook;
}



/* lpt_client_get_tree:
 */
LptTree *lpt_client_get_tree ( LptClient *client )
{
  return client->tree;
}



/* lpt_tree_set_message_handler:
 */
void lpt_tree_set_message_handler ( LptTree *tree,
                                    LptMessageHandler handler,
                                    gpointer data,
                                    GDestroyNotify destroy_data )
{
  /* [FIXME] destroy old data */
  tree->handler = handler;
  tree->handler_data = data;
  tree->destroy_handler_data = destroy_data;
}



/* lpt_tree_create_share:
 */
void lpt_tree_create_share ( LptTree *tree,
                             const gchar *name,
                             const gchar *path,
                             guint flags )
{
}



/* lpt_tree_connect_share:
 */
void lpt_tree_connect_share ( LptTree *tree,
                              LptClient *client,
                              const gchar *name,
                              const gchar *dest_path,
                              guint flags )
{
}



/* lpt_tree_add_client:
 */
LptClient *lpt_tree_add_client ( LptTree *tree,
                                 const gchar *name )
{
  LptClient *client = lpt_client_new(tree, name);
  tree->clients = g_list_append(tree->clients, client);
  return client;
}



/* _lpt_tree_set_node_value:
 */
void _lpt_tree_set_node_value ( LptTree *tree,
                                LptNode *node,
                                LObject *value )
{
  /* note the 'tree' may be nul here */
  ASSERT(node->tree == tree);
  if (tree)
    {
      LptEvent event;
      GList *l;
      /* create the event */
      event.type = 0; /* [fixme] */
      /* call hooks */
      for (l = tree->hooks; l; l = l->next)
        {
          LptHook *hook = l->data;
          hook->func(&event, hook->data);
        }
    }
  /* set the value */
  l_object_ref(value);
  if (node->value)
    L_OBJECT_CLEAR(node->value);
  node->value = value;
}
