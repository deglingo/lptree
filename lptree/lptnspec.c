/* lptnspec.c -
 */

#include "lptree/lptnspec.h"
#include "lptree/lptnspec.inl"



static void _dispose ( LObject *object );



/* lpt_nspec_class_init:
 */
static void lpt_nspec_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  g_free(LPT_NSPEC(object)->name);
  LPT_NSPEC(object)->name = NULL;
  ((LObjectClass *) parent_class)->dispose(object);
}
