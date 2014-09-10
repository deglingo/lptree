/* lptnspecdir.c -
 */

#include "lptree/lptnspecdir.h"
#include "lptree/lptnspecdir.inl"



/* lpt_nspec_dir_new:
 */
LptNSpec *lpt_nspec_dir_new ( const gchar *name )
{
  LptNSpecDir *nspec = LPT_NSPEC_DIR(l_object_new(LPT_CLASS_NSPEC_DIR, NULL));
  LPT_NSPEC(nspec)->name = g_strdup(name); /* [fixme] */
  return LPT_NSPEC(nspec);
}
