/* lptnspecint.h -
 */

#ifndef _LPTNSPECINT_H_
#define _LPTNSPECINT_H_

#include "lptree/lptnspec.h"
#include "lptree/lptnspecint-def.h"



/* LptNSpecInt:
 */
struct _LptNSpecInt
{
  LPT_NSPEC_INT_INSTANCE_HEADER;

  gint mini;
  gint maxi;
  gint defo;
};



/* LptNSpecIntClass:
 */
struct _LptNSpecIntClass
{
  LPT_NSPEC_INT_CLASS_HEADER;
};



LptNSpec *lpt_nspec_int_new ( const gchar *name,
                              gint mini,
                              gint maxi,
                              gint defo );



#endif
