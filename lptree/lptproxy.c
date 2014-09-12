/* lptproxy.c -
 */

#include "lptree/lptproxy.h"
#include "lptree/lptproxy.inl"



/* lpt_proxy_new:
 */
LptProxy *lpt_proxy_new ( LptTree *tree )
{
  LptProxy *proxy;
  proxy = LPT_PROXY(l_object_new(LPT_CLASS_PROXY, NULL));
  return proxy;
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
