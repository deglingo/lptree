/* lptbase.c -
 */

#include "lptree/lptbase.h"



/* lptree_init_once:
 */
static gpointer lptree_init_once ( gpointer dummy )
{
  los_init();
  return NULL;
}



/* lptree_init:
 */
void lptree_init ( void )
{
  GOnce once = G_ONCE_INIT;
  g_once(&once, lptree_init_once, NULL);
}
