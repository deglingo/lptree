/* lptnspec.h -
 */

#ifndef _LPTNSPEC_H_
#define _LPTNSPEC_H_

#include "lptree/lptbase.h"
#include "lptree/lptnspec-def.h"



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
};



guint lpt_nspec_get_id ( LptNSpec *nspec );



#endif
