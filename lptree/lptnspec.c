/* lptnspec.c -
 */

#include "lptree/lptnspec.h"
#include "lptree/lptnspec.inl"



static void _dispose ( LObject *object );

static guint nsid_counter = 1;



/* lpt_nspec_class_init:
 */
static void lpt_nspec_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* lpt_nspec_init:
 */
static void lpt_nspec_init ( LObject *object )
{
  LPT_NSPEC(object)->nsid = g_atomic_int_add(&nsid_counter, 1);
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  g_free(LPT_NSPEC(object)->name);
  LPT_NSPEC(object)->name = NULL;
  ((LObjectClass *) parent_class)->dispose(object);
}



/* lpt_nspec_get_id:
 */
guint lpt_nspec_get_id ( LptNSpec *nspec )
{
  return nspec->nsid;
}
