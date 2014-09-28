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
  gchar *str = l_object_to_string(msg);
  CL_DEBUG("[TODO] message: %s", str);
  g_free(str);
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
  msg = l_tuple_newl_give(1, l_int_new(1), NULL);
  proxy->handler(proxy, clid, L_OBJECT(msg), proxy->handler_data);
  l_object_unref(msg);
}
