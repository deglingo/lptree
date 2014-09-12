/* lptproxy.h -
 */

#ifndef _LPTPROXY_H_
#define _LPTPROXY_H_

#include "lptree/lptbase.h"
#include "lptree/lpttree.h"
#include "lptree/lptproxy-def.h"



/* LptProxy:
 */
struct _LptProxy
{
  LPT_PROXY_INSTANCE_HEADER;
};



/* LptProxyClass:
 */
struct _LptProxyClass
{
  LPT_PROXY_CLASS_HEADER;
};



LptProxy *lpt_proxy_new ( LptTree *tree );



#endif
