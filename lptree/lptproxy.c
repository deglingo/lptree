/* lptproxy.c -
 */

#include "lptree/private.h"
#include "lptree/lptproxy.h"
#include "lptree/lptproxy.inl"

/* [FIXME] */
#include "lptree/lptnspecdir.h"



/* LptProxyMessage:
 */
typedef enum _LptProxyMessage
  {
    LPT_PROXY_MESSAGE_REQUEST_CONNECT = 0,
    LPT_PROXY_MESSAGE_CONFIRM_CONNECT,
    LPT_PROXY_MESSAGE_COUNT,
  }
  LptProxyMessage;

static LInt *lpt_proxy_message_cache[LPT_PROXY_MESSAGE_COUNT] = { NULL, };

static LInt *lpt_proxy_message_ref ( LptProxyMessage msg )
{
  return l_object_ref(lpt_proxy_message_cache[msg]);
}



/* LptProxyClient:
 */
struct _LptProxyClient
{
  LptProxy *proxy;
};



/* Share:
 */
typedef struct _Share
  {
    guint shareid;
    gboolean owned;
    gchar *name;
    gchar *path;
    LptNode *root;
  }
  Share;

static guint shareid_counter = 1;



static void _dispose ( LObject *object );



/* share_new:
 */
static Share *share_new ( void )
{
  Share *share = g_new0(Share, 1);
  share->shareid = g_atomic_int_add(&shareid_counter, 1);
  return share;
}



/* share_free:
 */
static void share_free ( Share *share )
{
  g_free(share->path);
  g_free(share->name);
  g_free(share);
}



/* lpt_proxy_client_get_proxy:
 */
LptProxy *lpt_proxy_client_get_proxy ( LptProxyClient *client )
{
  return client->proxy;
}



/* lpt_proxy_class_init:
 */
static void lpt_proxy_class_init ( LObjectClass *cls )
{
  guint m;
  cls->dispose = _dispose;
  for (m = 0; m < LPT_PROXY_MESSAGE_COUNT; m++)
    lpt_proxy_message_cache[m] = l_int_new(m);
}



/* lpt_proxy_new:
 */
LptProxy *lpt_proxy_new ( LptTree *tree,
                          LptProxyHandler handler,
                          gpointer handler_data )
{
  LptProxy *proxy;
  proxy = LPT_PROXY(l_object_new(LPT_CLASS_PROXY, NULL));
  proxy->tree = l_object_ref(tree);
  proxy->handler = handler;
  proxy->handler_data = handler_data;
  proxy->shares = g_hash_table_new_full(NULL, NULL,
                                        NULL,
                                        (GDestroyNotify) share_free);
  proxy->shares_by_name = g_hash_table_new(g_str_hash, g_str_equal);
  return proxy;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LptProxy *proxy = LPT_PROXY(object);
  g_list_free_full(proxy->clients, g_free);
  proxy->clients = NULL;
  if (proxy->shares) {
    g_hash_table_unref(proxy->shares);
    proxy->shares = NULL;
  }
  if (proxy->shares_by_name) {
    g_hash_table_unref(proxy->shares_by_name);
    proxy->shares_by_name = NULL;
  }
  L_OBJECT_CLEAR(proxy->tree);
  ((LObjectClass *) parent_class)->dispose(object);
}



struct ntree_child_data
{
  LptProxy *proxy;
  LTuple *children;
  guint n_child;
};

static LTuple *_get_ntree ( LptProxy *proxy,
                            LptNode *node );



/* _get_ntree_child:
 */
static void _get_ntree_child ( LptNode *node,
                                  gpointer data_ )
{
  struct ntree_child_data *data = data_;
  l_tuple_give_item(data->children, data->n_child++,
                    L_OBJECT(_get_ntree(data->proxy, node)));
}



/* _get_ntree:
 */
static LTuple *_get_ntree ( LptProxy *proxy,
                            LptNode *node )
{
  /* node tuple: (nid, nspecid, key, children) */ 
  LTuple *ntree = l_tuple_new(4);
  LTuple *children;
  struct ntree_child_data data;
  l_tuple_give_item(ntree, 0, L_OBJECT(l_int_new(0))); /* [fixme] */
  l_tuple_give_item(ntree, 1, L_OBJECT(l_int_new(0))); /* [fixme] */
  l_tuple_give_item(ntree, 2, l_object_ref(node->key));
  children = l_tuple_new(lpt_node_get_n_children(node));
  data.proxy = proxy;
  data.children = children;
  data.n_child = 0;
  lpt_node_foreach(node, _get_ntree_child, &data);
  l_tuple_give_item(ntree, 3, L_OBJECT(children));
  return ntree;
}



/* /\* _handle_request_connect: */
/*  *\/ */
/* static void _handle_request_connect ( LptProxy *proxy, */
/*                                       LInt *clid, */
/*                                       LObject *msg ) */
/* { */
/*   LTuple *answer, *ntree; */
/*   LObject *shareid, *name; */
/*   Share *share; */
/*   ASSERT(L_TUPLE_SIZE(msg) == 4); */
/*   shareid = L_TUPLE_ITEM(msg, 2); */
/*   ASSERT(L_IS_INT(shareid)); */
/*   name = L_TUPLE_ITEM(msg, 3); */
/*   ASSERT(L_IS_STRING(name)); */
/*   share = g_hash_table_lookup(proxy->shares_by_name, L_STRING(name)->str); */
/*   ASSERT(share); */
/*   ntree = _get_ntree(proxy, share->root); */
/*   answer = l_tuple_newl_give(4, */
/*                              lpt_proxy_message_ref(LPT_PROXY_MESSAGE_CONFIRM_CONNECT), */
/*                              l_object_ref(clid), */
/*                              l_object_ref(shareid), */
/*                              ntree, */
/*                              NULL); */
/*   proxy->handler(proxy, L_INT_VALUE(clid), L_OBJECT(answer), proxy->handler_data); */
/*   l_object_unref(answer); */
/* } */



/* _create_ntree:
 */
static void _create_ntree ( LptProxy *proxy,
                            Share *share,
                            LTuple *ntree,
                            LptNode *base )
{
  LptNSpec *nspec = lpt_nspec_dir_new("DIR");
  guint c;
  LTuple *children;
  if (!base)
    {
      base = lpt_tree_create_node(proxy->tree, share->path, nspec);
    }
  else
    {
      LObject *key = L_TUPLE_ITEM(ntree, 2);
      LptNode *node = lpt_node_new(nspec);
      lpt_node_add(base, node, key);
      base = node;
      l_object_unref(node);
    }
  l_object_unref(nspec);
  children = L_TUPLE(L_TUPLE_ITEM(ntree, 3));
  for (c = 0; c < L_TUPLE_SIZE(children); c++)
    _create_ntree(proxy, share, L_TUPLE(L_TUPLE_ITEM(children, c)), base);
}



/* _handle_confirm_connect:
 */
static void _handle_confirm_connect ( LptProxy *proxy,
                                      LInt *clid,
                                      LObject *msg )
{
  LptNSpec *ns = lpt_nspec_dir_new("DIR");
  LObject *shareid, *ntree;
  Share *share;
  ASSERT(L_TUPLE_SIZE(msg) == 4);
  shareid = L_TUPLE_ITEM(msg, 2);
  ASSERT(L_IS_INT(shareid));
  ntree = L_TUPLE_ITEM(msg, 3);
  ASSERT(L_IS_TUPLE(ntree));
  share = g_hash_table_lookup(proxy->shares, GUINT_TO_POINTER(L_INT_VALUE(shareid)));
  ASSERT(share);
  _create_ntree(proxy, share, L_TUPLE(ntree), NULL);
  l_object_unref(ns);
}



/* lpt_proxy_handle_message:
 */
void lpt_proxy_handle_message ( LptProxy *proxy,
                                LptProxyClient *client,
                                LObject *msg )
{
  LObject *key;
  LInt *clid;
  /* CL_DEBUG("proxy_message(%s, %s)", */
  /*          l_object_to_string(L_OBJECT(proxy)), */
  /*          l_object_to_string(L_OBJECT(msg))); */
  ASSERT(L_IS_TUPLE(msg));
  ASSERT(L_TUPLE_SIZE(msg) >= 2);
  /* get key */
  key = L_TUPLE_ITEM(msg, 0);
  ASSERT(L_IS_INT(key));
  /* get clid */
  clid = L_INT(L_TUPLE_ITEM(msg, 1));
  ASSERT(L_IS_INT(clid));
  /* handle key */
  switch (L_INT_VALUE(key)) {
  case LPT_PROXY_MESSAGE_REQUEST_CONNECT:
    CL_DEBUG("[TODO] request_connect");
    /* _handle_request_connect(proxy, clid, msg); */
    break;
  case LPT_PROXY_MESSAGE_CONFIRM_CONNECT:
    _handle_confirm_connect(proxy, clid, msg);
    break;
  default:
    CL_DEBUG("[TODO] msg key: %d", L_INT_VALUE(key));
  }
}



/* lpt_proxy_create_share:
 */
void lpt_proxy_create_share ( LptProxy *proxy,
                              const gchar *name,
                              const gchar *path,
                              gint flags )
{
  Share *share = share_new();
  share->name = g_strdup(name);
  share->path = g_strdup(path);
  share->owned = TRUE;
  share->root = lpt_tree_get_node(proxy->tree, path);
  ASSERT(share->root);
  g_hash_table_insert(proxy->shares, GUINT_TO_POINTER(share->shareid), share);
  g_hash_table_insert(proxy->shares_by_name, share->name, share);
}



/* lpt_proxy_connect_client:
 */
void lpt_proxy_connect_client ( LptProxy *proxy,
                                guint clid )
{
}



/* lpt_proxy_create_client:
 */
LptProxyClient *lpt_proxy_create_client ( LptProxy *proxy )
{
  LptProxyClient *cli = g_new0(LptProxyClient, 1);
  proxy->clients = g_list_append(proxy->clients, cli);
  cli->proxy = proxy;
  return cli;
}



/* lpt_proxy_connect_share:
 */
void lpt_proxy_connect_share ( LptProxy *proxy,
                               LptProxyClient *client,
                               const gchar *share_name,
                               const gchar *dest_path,
                               gint flags )
{
  Share *share;
  LTuple *msg;
  share = share_new();
  share->owned = FALSE;
  share->name = g_strdup(share_name); /* ?? */
  share->path = g_strdup(dest_path);
  g_hash_table_insert(proxy->shares, GUINT_TO_POINTER(share->shareid), share);
  msg = l_tuple_newl_give(3,
                          lpt_proxy_message_ref(LPT_PROXY_MESSAGE_REQUEST_CONNECT),
                          l_int_new(share->shareid),
                          l_string_new(share_name),
                          NULL);
  proxy->handler(proxy, client, L_OBJECT(msg), proxy->handler_data);
  l_object_unref(msg);
}
