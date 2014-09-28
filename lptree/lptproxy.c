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



static void _dispose ( LObject *object );



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
  return proxy;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(LPT_PROXY(object)->tree);
  ((LObjectClass *) parent_class)->dispose(object);
}



/* _handle_request_connect:
 */
static void _handle_request_connect ( LptProxy *proxy,
                                      LInt *clid,
                                      LObject *msg )
{
  LTuple *answer = l_tuple_newl_give(2,
                                     lpt_proxy_message_ref(LPT_PROXY_MESSAGE_CONFIRM_CONNECT),
                                     l_object_ref(clid),
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
  lpt_tree_create_node(proxy->tree, "/dest-share1", ns);
  l_object_unref(ns);
}



/* lpt_proxy_handle_message:
 */
void lpt_proxy_handle_message ( LptProxy *proxy,
                                LObject *msg )
{
  LObject *key;
  LInt *clid;
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
  LTuple *msg;
  msg = l_tuple_newl_give(2,
                          lpt_proxy_message_ref(LPT_PROXY_MESSAGE_REQUEST_CONNECT),
                          l_int_new(clid),
                          NULL);
  proxy->handler(proxy, clid, L_OBJECT(msg), proxy->handler_data);
  l_object_unref(msg);
}
