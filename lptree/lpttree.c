/* lpttree.c -
 */

#include "lptree/private.h"
#include "lptree/lpttree.h"
#include "lptree/lptnspecdir.h"
#include "lptree/lptpath.h"
#include "lptree/lpttree.inl"

#include <string.h>



/* LptMessage:
 */
enum
  {
    LPT_MESSAGE_CONNECT_REQUEST,
    LPT_MESSAGE_CONNECT_ACCEPT,
  };



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



/* LptShare:
 */
typedef struct _LptShare
{
  guint shareid;
  gchar *name;
  gchar *path;
  LptNode *root;
}
  LptShare;

static volatile guint shareid_counter = 1;



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



/* lpt_share_new:
 */
static LptShare *lpt_share_new ( const gchar *name,
                                 const gchar *path )
{
  LptShare *share = g_new0(LptShare, 1);
  share->shareid = g_atomic_int_add(&shareid_counter, 1);
  if (name) share->name = g_strdup(name);
  share->path = g_strdup(path);
  return share;
}



/* lpt_share_free:
 */
static void lpt_share_free ( LptShare *share )
{
  g_free(share->name);
  g_free(share->path);
  g_free(share);
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
  tree->shares_by_id = g_hash_table_new(NULL, NULL);
  tree->shares_by_name = g_hash_table_new(g_str_hash, g_str_equal);
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
  g_list_free_full(tree->shares, (GDestroyNotify) lpt_share_free);
  tree->shares = NULL;
  g_hash_table_unref(tree->shares_by_id);
  tree->shares_by_id = NULL;
  g_hash_table_unref(tree->shares_by_name);
  tree->shares_by_name = NULL;
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



static void _unpack_message ( LObject *message,
                              const gchar *format,
                              ... )
  G_GNUC_NULL_TERMINATED;

static void _unpack_message ( LObject *message,
                              const gchar *format,
                              ... )
{
  guint len = strlen(format) + 1;
  guint i;
  const gchar *f;
  va_list args;
  LObject **item;
  ASSERT(message);
  ASSERT(L_IS_TUPLE(message));
  ASSERT(L_TUPLE_SIZE(message) == len);
  va_start(args, format);
  for (i = 1, f = format; i < len; i++, f++) {
    item = va_arg(args, LObject **);
    *item = L_TUPLE_ITEM(message, i);
    switch (*f) {
    case 'i': ASSERT(L_IS_INT(*item)); break;
    case 's': ASSERT(L_IS_STRING(*item)); break;
    case 't': ASSERT(L_IS_TUPLE(*item)); break;
    default: CL_ERROR("bad format: '%c'", *f);
    }
  }
  item = va_arg(args, LObject **);
  ASSERT(!item);
  va_end(args);
}



struct pack_ntree_data
{
  LptTree *tree;
  LTuple *children;
  guint n_children;
};



static void _pack_ntree_child ( LptNode *node,
                                gpointer data_ )
{
  struct pack_ntree_data *data = data_;
  /* pack tuple: ( nid, nspec, key, value, children ) */
  LTuple *pack = l_tuple_new(5);
  LTuple *children, *data_children;
  guint data_n_children;
  l_tuple_give_item(pack, 0, L_OBJECT(l_int_new(0))); /* nid */
  l_tuple_give_item(pack, 1, L_OBJECT(l_int_new(0))); /* nspecid */
  l_tuple_give_item(pack, 2, l_object_ref(node->key));
  l_tuple_give_item(pack, 3, L_OBJECT(l_int_new(0))); /* value */
  children = l_tuple_new(lpt_node_get_n_children(node));
  data_children = data->children;
  data_n_children = data->n_children;
  data->children = children;
  data->n_children = 0;
  lpt_node_foreach(node, _pack_ntree_child, data);
  l_tuple_give_item(pack, 4, L_OBJECT(children));
  data->children = data_children;
  data->n_children = data_n_children;
  l_tuple_give_item(data->children, data->n_children++, L_OBJECT(pack));
}



static LObject *_pack_ntree ( LptTree *tree,
                              LptNode *node )
{
  struct pack_ntree_data data;
  LObject *pack;
  data.tree = tree;
  data.children = l_tuple_new(1);
  data.n_children = 0;
  _pack_ntree_child(node, &data);
  pack = l_object_ref(L_TUPLE_ITEM(data.children, 0));
  l_object_unref(data.children);
  return pack;
}



static void _unpack_ntree ( LptTree *tree,
                            LptShare *share,
                            LptNode *base,
                            LTuple *ntree )
{
  LInt *nid, *nspecid;
  LObject *key, *value;
  LTuple *children;
  LptNSpec *nspec = lpt_nspec_dir_new("DIR");
  guint i;
  ASSERT(L_TUPLE_SIZE(ntree) == 5);
  nid = L_INT(L_TUPLE_ITEM(ntree, 0));
  nspecid = L_INT(L_TUPLE_ITEM(ntree, 1));
  key = L_TUPLE_ITEM(ntree, 2);
  value = L_TUPLE_ITEM(ntree, 3);
  children = L_TUPLE(L_TUPLE_ITEM(ntree, 4));
  if (!base)
    {
      base = lpt_tree_create_node(tree, share->path, nspec);
    }
  else
    {
      LptNode *node = lpt_node_new(nspec);
      lpt_node_add(base, node, key);
      l_object_unref(node);
      base = node;
    }
  /* children */
  for (i = 0; i < L_TUPLE_SIZE(children); i++)
    _unpack_ntree(tree, share, base, L_TUPLE(L_TUPLE_ITEM(children, i)));
  /* [fixme] */
  l_object_unref(nspec);
}



/* _handle_connect_request:
 */
static void _handle_connect_request ( LptTree *tree,
                                      LptClient *client,
                                      LObject *message )
{
  LInt *shareid;
  LString *name;
  LTuple *answer;
  LObject *ntree;
  LptShare *share;
  _unpack_message(message, "is", &shareid, &name, NULL);
  share = g_hash_table_lookup(tree->shares_by_name, L_STRING(name)->str);
  ASSERT(share);
  ntree = _pack_ntree(tree, share->root);
  answer = l_tuple_new(4);
  l_tuple_give_item(answer, 0, L_OBJECT(l_int_new(LPT_MESSAGE_CONNECT_ACCEPT)));
  l_tuple_give_item(answer, 1, l_object_ref(shareid));
  l_tuple_give_item(answer, 2, L_OBJECT(l_tuple_new(0))); /* nspecs */
  l_tuple_give_item(answer, 3, ntree);
  tree->handler(tree, client, L_OBJECT(answer), tree->handler_data);
  l_object_unref(answer);
}



/* _handle_connect_accept:
 */
static void _handle_connect_accept ( LptTree *tree,
                                      LptClient *client,
                                      LObject *message )
{
  LInt *shareid;
  LTuple *nspecs, *ntree;
  LptShare *share;
  /* CL_DEBUG("connect_accept: %s", l_object_to_string(message)); */
  _unpack_message(message, "itt", &shareid, &nspecs, &ntree, NULL);
  share = g_hash_table_lookup(tree->shares_by_id,
                              GUINT_TO_POINTER(L_INT_VALUE(shareid)));
  ASSERT(share);
  _unpack_ntree(tree, share, NULL, ntree);
}



/* lpt_tree_handle_message:
 */
void lpt_tree_handle_message ( LptTree *tree,
                               LptClient *client,
                               LObject *message )
{
  ASSERT(tree);
  ASSERT(client);
  ASSERT(client->tree == tree);
  ASSERT(message);
  ASSERT(L_IS_TUPLE(message));
  ASSERT(L_TUPLE_SIZE(message) >= 1);
  ASSERT(L_IS_INT(L_TUPLE_ITEM(message, 0)));
  switch (L_INT_VALUE(L_TUPLE_ITEM(message, 0)))
    {
    case LPT_MESSAGE_CONNECT_REQUEST:
      _handle_connect_request(tree, client, message);
      break;
    case LPT_MESSAGE_CONNECT_ACCEPT:
      _handle_connect_accept(tree, client, message);
      break;
    default:
      CL_DEBUG("[TODO] message: %s", l_object_to_string(message));
    }
}



/* lpt_tree_create_share:
 */
void lpt_tree_create_share ( LptTree *tree,
                             const gchar *name,
                             const gchar *path,
                             guint flags )
{
  LptShare *share;
  share = lpt_share_new(name, path);
  share->root = lpt_tree_get_node(tree, path); /* [fixme] ref ? */
  ASSERT(share->root);
  tree->shares = g_list_append(tree->shares, share);
  g_hash_table_insert(tree->shares_by_id, GUINT_TO_POINTER(share->shareid), share);
  g_hash_table_insert(tree->shares_by_name, share->name, share);
}



/* lpt_tree_connect_share:
 */
void lpt_tree_connect_share ( LptTree *tree,
                              LptClient *client,
                              const gchar *name,
                              const gchar *dest_path,
                              guint flags )
{
  LTuple *msg;
  LptShare *share;
  share = lpt_share_new(NULL /* ?? */, dest_path);
  tree->shares = g_list_append(tree->shares, share);
  g_hash_table_insert(tree->shares_by_id,
                      GUINT_TO_POINTER(share->shareid),
                      share);
  msg = l_tuple_new(3);
  l_tuple_give_item(msg, 0, L_OBJECT(l_int_new(LPT_MESSAGE_CONNECT_REQUEST)));
  l_tuple_give_item(msg, 1, L_OBJECT(l_int_new(share->shareid)));
  l_tuple_give_item(msg, 2, L_OBJECT(l_string_new(name)));
  tree->handler(tree, client, L_OBJECT(msg), tree->handler_data);
  l_object_unref(msg);
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
