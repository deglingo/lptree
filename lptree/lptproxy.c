/* lptproxy.c -
 */

#include "lptree/private.h"
#include "lptree/lptproxy.h"
#include "lptree/lptproxy.inl"



/* lpt_proxy_new:
 */
LptProxy *lpt_proxy_new ( LptTree *tree,
                          LptProxyHandler handler,
                          gpointer handler_data )
{
  LptProxy *proxy;
  proxy = LPT_PROXY(l_object_new(LPT_CLASS_PROXY, NULL));
  proxy->handler = handler;
  proxy->handler_data = handler_data;
  return proxy;
}



/* lpt_proxy_handle_message:
 */
void lpt_proxy_handle_message ( LptProxy *proxy,
                                LObject *msg )
{
  LObject *key;
  guint clid;
  ASSERT(L_IS_TUPLE(msg));
  ASSERT(L_TUPLE_SIZE(msg) >= 2);
  key = L_TUPLE_ITEM(msg, 0);
  ASSERT(L_IS_INT(key));
  ASSERT(L_IS_INT(L_TUPLE_ITEM(msg, 1)));
  clid = L_INT_VALUE(L_TUPLE_ITEM(msg, 1));
  switch (L_INT_VALUE(key))
    {
    case 1:
      {
        LTuple *answer = l_tuple_newl_give(2, l_int_new(2), l_int_new(clid), NULL);
        proxy->handler(proxy, clid, L_OBJECT(answer), proxy->handler_data);
        l_object_unref(answer);
      }
      break;
    default:
      CL_DEBUG("[TODO] message key: %d", L_INT_VALUE(key));
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
  msg = l_tuple_newl_give(2, l_int_new(1), l_int_new(clid), NULL);
  proxy->handler(proxy, clid, L_OBJECT(msg), proxy->handler_data);
  l_object_unref(msg);
}
