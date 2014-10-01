/* lptnspecint.c -
 */

#include "lptree/private.h"
#include "lptree/lptnspecint.h"
#include "lptree/lptnode.h"
#include "lptree/lptnspecint.inl"



static LObject *_get_state ( LObject *object );
static LObject *_from_state ( LObjectClass *cls,
                              LObject *state );
static LptNode *_create_node ( LptNSpec *nspec );



/* lpt_nspec_int_class_init:
 */
static void lpt_nspec_int_class_init ( LObjectClass *cls )
{
  cls->get_state = _get_state;
  cls->from_state = _from_state;
  LPT_NSPEC_CLASS(cls)->value_type = l_object_ref(L_CLASS_INT);
  LPT_NSPEC_CLASS(cls)->create_node = _create_node;
}



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



/* _get_state:
 */
static LObject *_get_state ( LObject *object )
{
  LObject *parent = ((LObjectClass *) parent_class)->get_state(object);
  LTuple *t = l_tuple_newl_give(4,
                                parent,
                                l_int_new(LPT_NSPEC_INT(object)->mini),
                                l_int_new(LPT_NSPEC_INT(object)->maxi),
                                l_int_new(LPT_NSPEC_INT(object)->defo),
                                NULL);
  return L_OBJECT(t);
}



/* _from_state:
 */
static LObject *_from_state ( LObjectClass *cls,
                              LObject *state )
{
  ASSERT(L_IS_TUPLE(state));
  ASSERT(L_TUPLE_SIZE(state) == 4);
  LObject *nspec = ((LObjectClass *) parent_class)->from_state(cls, L_TUPLE_ITEM(state, 0));
  LPT_NSPEC_INT(nspec)->mini = L_INT_VALUE(L_TUPLE_ITEM(state, 1));
  LPT_NSPEC_INT(nspec)->maxi = L_INT_VALUE(L_TUPLE_ITEM(state, 2));
  LPT_NSPEC_INT(nspec)->defo = L_INT_VALUE(L_TUPLE_ITEM(state, 3));
  return nspec;
}



/* _create_node:
 */
static LptNode *_create_node ( LptNSpec *nspec )
{
  LptNode *n = LPT_NODE(l_object_new(LPT_CLASS_NODE, NULL));
  n->nspec = l_object_ref(nspec);
  n->value = L_OBJECT(l_int_new(LPT_NSPEC_INT(nspec)->defo));
  return n;
}
