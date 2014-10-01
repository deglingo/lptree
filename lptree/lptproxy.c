/* lptproxy.c -
 */

#include "lptree/private.h"
#include "lptree/lptproxy.h"
#include "lptree/lptproxy.inl"

/* [FIXME] */
#include "lptree/lptnspecdir.h"

#include <string.h>



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
  LDict *nspecs_map;
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



/* lpt_proxy_client_free:
 */
static void lpt_proxy_client_free ( LptProxyClient *client )
{
  l_object_unref(client->nspecs_map);
  g_free(client);
}



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
  proxy->nspecs = l_dict_new(); /* map <nsid, NSpec> */
  return proxy;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LptProxy *proxy = LPT_PROXY(object);
  g_list_free_full(proxy->clients, (GDestroyNotify) lpt_proxy_client_free);
  proxy->clients = NULL;
  if (proxy->shares) {
    g_hash_table_unref(proxy->shares);
    proxy->shares = NULL;
  }
  if (proxy->shares_by_name) {
    g_hash_table_unref(proxy->shares_by_name);
    proxy->shares_by_name = NULL;
  }
  L_OBJECT_CLEAR(proxy->nspecs);
  L_OBJECT_CLEAR(proxy->tree);
  ((LObjectClass *) parent_class)->dispose(object);
}



struct ntree_child_data
{
  LptProxy *proxy;
  LTuple *children;
  guint n_child;
  LDict *nspec_map;
  LList *nspec_list;
};

static LTuple *_get_ntree ( LptProxy *proxy,
                            LptNode *node,
                            LDict *nspec_map,
                            LList *nspec_list );



/* _get_nspec_state:
 */
static LObject *_get_nspec_state ( LptNSpec *nspec,
                                   LInt *nsid )
{
  LTuple *ns = l_tuple_new(3);
  l_tuple_give_item(ns, 0, L_OBJECT(l_string_new(l_object_class_name(L_OBJECT_GET_CLASS(nspec)))));
  l_tuple_give_item(ns, 1, l_object_ref(nsid));
  l_tuple_give_item(ns, 2, l_object_get_state(L_OBJECT(nspec)));
  return L_OBJECT(ns);
}



/* _get_ntree_child:
 */
static void _get_ntree_child ( LptNode *node,
                               gpointer data_ )
{
  struct ntree_child_data *data = data_;
  l_tuple_give_item(data->children, data->n_child++,
                    L_OBJECT(_get_ntree(data->proxy, node, data->nspec_map, data->nspec_list)));
}



/* _get_ntree:
 */
static LTuple *_get_ntree ( LptProxy *proxy,
                            LptNode *node,
                            LDict *nspec_map,
                            LList *nspec_list )
{
  /* node tuple: (nid, nspecid, key, value, children) */ 
  LTuple *ntree = l_tuple_new(5);
  LTuple *children;
  LptNSpec *nspec = lpt_node_get_nspec(node);
  LInt *nspecid = l_int_new(lpt_nspec_get_id(nspec));
  struct ntree_child_data data;
  /* get nspec_id */
  if (!l_dict_lookup(nspec_map, L_OBJECT(nspecid))) {
    LObject *state = _get_nspec_state(nspec, nspecid);
    /* [fixme] nspec_map should be a set */
    l_dict_insert(nspec_map, L_OBJECT(nspecid), L_OBJECT(nspec));
    l_list_append(nspec_list, state);
    l_object_unref(state);
  }
  l_tuple_give_item(ntree, 0, L_OBJECT(l_int_new(0))); /* [fixme] */
  l_tuple_give_item(ntree, 1, l_object_ref(nspecid));
  l_tuple_give_item(ntree, 2, l_object_ref(node->key));
  /* value */
  if (lpt_nspec_get_value_type(nspec)) {
    l_tuple_give_item(ntree, 3, l_object_ref(lpt_node_get_value(node)));;
  } else {
    l_tuple_give_item(ntree, 3, L_OBJECT(l_none_ref()));
  }
  /* children */
  children = l_tuple_new(lpt_node_get_n_children(node));
  data.proxy = proxy;
  data.children = children;
  data.n_child = 0;
  data.nspec_map = nspec_map;
  data.nspec_list = nspec_list;
  lpt_node_foreach(node, _get_ntree_child, &data);
  l_tuple_give_item(ntree, 4, L_OBJECT(children));
  l_object_unref(nspecid);
  return ntree;
}



static void _msg_check ( LObject *msg,
                         const gchar *format,
                         ... )
{
  guint size = strlen(format) + 1; /* + key */
  guint i;
  const gchar *f;
  va_list args;
  ASSERT(L_IS_TUPLE(msg));
  ASSERT(L_TUPLE_SIZE(msg) == size);
  va_start(args, format);
  for (i = 1, f = format; i < size; i++, f++) {
    LObject **dest = va_arg(args, LObject **);
    *dest = L_TUPLE_ITEM(msg, i);
    switch (*f) {
    case 'i': ASSERT(L_IS_INT(*dest)); break;
    case 's': ASSERT(L_IS_STRING(*dest)); break;
    case 't': ASSERT(L_IS_TUPLE(*dest)); break;
    default: ASSERT(0);
    }
  }
  va_end(args);
}



/* _handle_request_connect:
 */
static void _handle_request_connect ( LptProxy *proxy,
                                      LptProxyClient *client,
                                      LObject *msg )
{
  LInt *shareid;
  LString *name;
  _msg_check(msg, "is", &shareid, &name);
  /* CL_DEBUG("request_connect: %s", l_object_to_string(msg)); */
  LTuple *answer, *ntree;
  Share *share;
  LList *nspecs_list = l_list_new();
  LTuple *nspecs_tuple;
  share = g_hash_table_lookup(proxy->shares_by_name, L_STRING(name)->str);
  ASSERT(share);
  ntree = _get_ntree(proxy, share->root, client->nspecs_map, nspecs_list);
  nspecs_tuple = l_tuple_new_from_list(nspecs_list);
  l_object_unref(nspecs_list);
  answer = l_tuple_newl_give(4,
                             lpt_proxy_message_ref(LPT_PROXY_MESSAGE_CONFIRM_CONNECT),
                             l_object_ref(shareid),
                             nspecs_tuple,
                             ntree,
                             NULL);
  proxy->handler(proxy, client, L_OBJECT(answer), proxy->handler_data);
  l_object_unref(answer);
}



/* _create_ntree:
 */
static void _create_ntree ( LptProxy *proxy,
                            Share *share,
                            LTuple *ntree,
                            LptNode *base )
{
  LptNSpec *nspec;
  guint c;
  LObject *nid, *nsid, *key, *value, *children;
  /* CL_ERROR("ntree: %s", l_object_to_string(L_OBJECT(ntree))); */
  nid = L_TUPLE_ITEM(ntree, 0);
  nsid = L_TUPLE_ITEM(ntree, 1);
  key = L_TUPLE_ITEM(ntree, 2);
  value = L_TUPLE_ITEM(ntree, 3);
  children = L_TUPLE_ITEM(ntree, 4);
  nspec = LPT_NSPEC(l_dict_lookup(proxy->nspecs, nsid));
  ASSERT(nspec);
  if (!base)
    {
      base = lpt_tree_create_node(proxy->tree, share->path, nspec);
    }
  else
    {
      LptNode *node = lpt_node_new(nspec);
      lpt_node_add(base, node, key);
      base = node;
      l_object_unref(node);
    }
  /* set value */
  if (lpt_nspec_get_value_type(nspec))
    lpt_node_set_value(base, value);
  /* l_object_unref(nspec); */
  for (c = 0; c < L_TUPLE_SIZE(children); c++)
    _create_ntree(proxy, share, L_TUPLE(L_TUPLE_ITEM(children, c)), base);
}



/* _handle_confirm_connect:
 */
static void _handle_confirm_connect ( LptProxy *proxy,
                                      LptProxyClient *client,
                                      LObject *msg )
{
  LptNSpec *ns = lpt_nspec_dir_new("DIR");
  LObject *shareid, *nspecs, *ntree;
  Share *share;
  guint i;
  /* CL_DEBUG("confim_connect: %s", l_object_to_string(msg)); */
  _msg_check(msg, "itt", &shareid, &nspecs, &ntree);
  share = g_hash_table_lookup(proxy->shares, GUINT_TO_POINTER(L_INT_VALUE(shareid)));
  ASSERT(share);
  /* register the nspecs */
  for (i = 0; i < L_TUPLE_SIZE(nspecs); i++) {
    LTuple *ns = L_TUPLE(L_TUPLE_ITEM(nspecs, i));
    const gchar *clsname;
    LObject *nsid, *state, *nspec;
    ASSERT(L_TUPLE_SIZE(ns) == 3);
    clsname = L_STRING(L_TUPLE_ITEM(ns, 0))->str;
    nsid = L_TUPLE_ITEM(ns, 1);
    state = L_TUPLE_ITEM(ns, 2);
    nspec = l_object_new_from_state(l_object_class_from_name(clsname), state);
    l_dict_insert(proxy->nspecs, nsid, nspec);
    l_object_unref(nspec);
  }
  /* create the tree */
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
  /* CL_DEBUG("proxy_message(%s, %s)", */
  /*          l_object_to_string(L_OBJECT(proxy)), */
  /*          l_object_to_string(L_OBJECT(msg))); */
  ASSERT(L_IS_TUPLE(msg));
  ASSERT(L_TUPLE_SIZE(msg) >= 1);
  /* get key */
  key = L_TUPLE_ITEM(msg, 0);
  ASSERT(L_IS_INT(key));
  /* handle key */
  switch (L_INT_VALUE(key)) {
  case LPT_PROXY_MESSAGE_REQUEST_CONNECT:
    _handle_request_connect(proxy, client, msg);
    break;
  case LPT_PROXY_MESSAGE_CONFIRM_CONNECT:
    _handle_confirm_connect(proxy, client, msg);
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
  cli->nspecs_map = l_dict_new();
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
