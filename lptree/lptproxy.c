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
