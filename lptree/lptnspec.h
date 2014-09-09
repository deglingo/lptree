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

  gchar *name;
};



/* LptNSpecClass:
 */
struct _LptNSpecClass
{
  LPT_NSPEC_CLASS_HEADER;
};



#endif
