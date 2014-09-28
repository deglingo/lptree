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



/* Share:
 */
typedef struct _Share
  {
    guint shareid;
    gchar *dest_path;
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
  g_free(share->dest_path);
  g_free(share);
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
  return proxy;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LptProxy *proxy = LPT_PROXY(object);
  if (proxy->shares) {
    g_hash_table_unref(proxy->shares);
    proxy->shares = NULL;
  }
  L_OBJECT_CLEAR(proxy->tree);
  ((LObjectClass *) parent_class)->dispose(object);
}



/* _handle_request_connect:
 */
static void _handle_request_connect ( LptProxy *proxy,
                                      LInt *clid,
                                      LObject *msg )
{
  LTuple *answer;
  LObject *shareid;
  ASSERT(L_TUPLE_SIZE(msg) == 3);
  shareid = L_TUPLE_ITEM(msg, 2);
  ASSERT(L_IS_INT(shareid));
  answer = l_tuple_newl_give(3,
                             lpt_proxy_message_ref(LPT_PROXY_MESSAGE_CONFIRM_CONNECT),
                             l_object_ref(clid),
                             l_object_ref(shareid),
                             NULL);
  proxy->handler(proxy, L_INT_VALUE(clid), L_OBJECT(answer), proxy->handler_data);
  l_object_unref(answer);
}



/* _handle_confirm_connect:
 */
static void _handle_confirm_connect ( LptProxy *proxy,
                                      LInt *clid,
                                      LObject *msg )
{
  LptNSpec *ns = lpt_nspec_dir_new("DIR");
  LObject *shareid;
  Share *share;
  ASSERT(L_TUPLE_SIZE(msg) == 3);
  shareid = L_TUPLE_ITEM(msg, 2);
  ASSERT(L_IS_INT(shareid));
  share = g_hash_table_lookup(proxy->shares, GUINT_TO_POINTER(L_INT_VALUE(shareid)));
  ASSERT(share);
  lpt_tree_create_node(proxy->tree, share->dest_path, ns);
  l_object_unref(ns);
}



/* lpt_proxy_handle_message:
 */
void lpt_proxy_handle_message ( LptProxy *proxy,
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
    _handle_request_connect(proxy, clid, msg);
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
}



/* lpt_proxy_connect_client:
 */
void lpt_proxy_connect_client ( LptProxy *proxy,
                                guint clid )
{
}



/* lpt_proxy_connect_share:
 */
void lpt_proxy_connect_share ( LptProxy *proxy,
                               guint clid,
                               const gchar *share_name,
                               const gchar *dest_path,
                               gint flags )
{
  Share *share;
  LTuple *msg;
  share = share_new();
  share->dest_path = g_strdup(dest_path);
  g_hash_table_insert(proxy->shares, GUINT_TO_POINTER(share->shareid), share);
  msg = l_tuple_newl_give(3,
                          lpt_proxy_message_ref(LPT_PROXY_MESSAGE_REQUEST_CONNECT),
                          l_int_new(clid),
                          l_int_new(share->shareid),
                          NULL);
  proxy->handler(proxy, clid, L_OBJECT(msg), proxy->handler_data);
  l_object_unref(msg);
}
