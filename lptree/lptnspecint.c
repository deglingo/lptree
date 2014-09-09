/* lptnspecint.c -
 */

#include "lptree/lptnspecint.h"
#include "lptree/lptnspecint.inl"



/* lpt_nspec_int_new:
 */
LptNSpec *lpt_nspec_int_new ( const gchar *name,
                              gint mini,
                              gint maxi,
                              gint defo )
{
  LptNSpecInt *nspec;
  nspec = LPT_NSPEC_INT(l_object_new(LPT_CLASS_NSPEC_INT, NULL));
  /* [fixme] */
  LPT_NSPEC(nspec)->name = g_strdup(name);
  nspec->mini = mini;
  nspec->maxi = maxi;
  nspec->defo = defo;
  return LPT_NSPEC(nspec);
}
