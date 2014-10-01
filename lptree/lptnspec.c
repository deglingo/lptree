/* lptnspec.c -
 */

#include "lptree/private.h"
#include "lptree/lptnspec.h"
#include "lptree/lptnspec.inl"



static void _dispose ( LObject *object );
static LObject *_get_state ( LObject *object );
static LObject *_from_state ( LObjectClass *cls,
                              LObject *state );

static guint nsid_counter = 1;



/* lpt_nspec_class_init:
 */
static void lpt_nspec_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  cls->get_state = _get_state;
  cls->from_state = _from_state;
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



/* _get_state:
 */
static LObject *_get_state ( LObject *object )
{
  return L_OBJECT(l_tuple_newl_give(1,
                                    l_string_new(LPT_NSPEC(object)->name),
                                    NULL));
}



/* _from_state:
 */
static LObject *_from_state ( LObjectClass *cls,
                              LObject *state )
{
  LObject *nspec;
  ASSERT(L_IS_TUPLE(state));
  ASSERT(L_TUPLE_SIZE(state) == 1);
  nspec = l_object_new(cls, NULL);
  LPT_NSPEC(nspec)->name = g_strdup(L_STRING(L_TUPLE_ITEM(state, 0))->str);
  return nspec;
}



/* lpt_nspec_get_id:
 */
guint lpt_nspec_get_id ( LptNSpec *nspec )
{
  return nspec->nsid;
}



/* lpt_nspec_get_value_type:
 */
LObjectClass *lpt_nspec_get_value_type ( LptNSpec *nspec )
{
  return LPT_NSPEC_GET_CLASS(nspec)->value_type;
}
