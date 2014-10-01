/* lptnspecdir.c -
 */

#include "lptree/lptnspecdir.h"
#include "lptree/lptnode.h"
#include "lptree/lptnspecdir.inl"



static LptNode *_create_node ( LptNSpec *nspec );



/* lpt_nspec_dir_class_init:
 */
static void lpt_nspec_dir_class_init ( LObjectClass *cls )
{
  LPT_NSPEC_CLASS(cls)->create_node = _create_node;
}



/* lpt_nspec_dir_new:
 */
LptNSpec *lpt_nspec_dir_new ( const gchar *name )
{
  LptNSpecDir *nspec = LPT_NSPEC_DIR(l_object_new(LPT_CLASS_NSPEC_DIR, NULL));
  LPT_NSPEC(nspec)->name = g_strdup(name); /* [fixme] */
  return LPT_NSPEC(nspec);
}



/* _create_node:
 */
static LptNode *_create_node ( LptNSpec *nspec )
{
  LptNode *n = LPT_NODE(l_object_new(LPT_CLASS_NODE, NULL));
  n->nspec = l_object_ref(nspec);
  return n;
}
