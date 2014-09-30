/* lptproxy.h -
 */

#ifndef _LPTPROXY_H_
#define _LPTPROXY_H_

#include "lptree/lptbase.h"
#include "lptree/lpttree.h"
#include "lptree/lptproxy-def.h"



typedef struct _LptProxyClient LptProxyClient;

typedef void (* LptProxyHandler) ( LptProxy *proxy,
                                   LptProxyClient *client,
                                   LObject *msg,
                                   gpointer data );



/* LptProxy:
 */
struct _LptProxy
{
  LPT_PROXY_INSTANCE_HEADER;

  LptTree *tree;
  LptProxyHandler handler;
  gpointer handler_data;
  GList *clients;
  GHashTable *shares; /* map <shareid, Share> */
  GHashTable *shares_by_name; /* map <gchar *name, Share> */
};



/* LptProxyClass:
 */
struct _LptProxyClass
{
  LPT_PROXY_CLASS_HEADER;
};



LptProxy *lpt_proxy_client_get_proxy ( LptProxyClient *client );

LptProxy *lpt_proxy_new ( LptTree *tree,
                          LptProxyHandler handler,
                          gpointer handler_data );
void lpt_proxy_handle_message ( LptProxy *proxy,
                                LptProxyClient *client,
                                LObject *msg );
void lpt_proxy_create_share ( LptProxy *proxy,
                              const gchar *name,
                              const gchar *path,
                              gint flags );
void lpt_proxy_connect_client ( LptProxy *proxy,
                                guint clid );
LptProxyClient *lpt_proxy_create_client ( LptProxy *proxy );
void lpt_proxy_connect_share ( LptProxy *proxy,
                               LptProxyClient *client,
                               const gchar *share_name,
                               const gchar *dest_path,
                               gint flags );



#endif
