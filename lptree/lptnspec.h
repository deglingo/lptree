/* lptnspec.h -
 */

#ifndef _LPTNSPEC_H_
#define _LPTNSPEC_H_

#include "lptree/lptbase.h"
#include "lptree/lptnspec-def.h"

struct _LptNode;



/* LptNSpec:
 */
struct _LptNSpec
{
  LPT_NSPEC_INSTANCE_HEADER;

  guint nsid;
  gchar *name;
};



/* LptNSpecClass:
 */
struct _LptNSpecClass
{
  LPT_NSPEC_CLASS_HEADER;

  LObjectClass *value_type;

  struct _LptNode * (* create_node) ( LptNSpec *nspec );
};



guint lpt_nspec_get_id ( LptNSpec *nspec );
LObjectClass *lpt_nspec_get_value_type ( LptNSpec *nspec );



#endif
