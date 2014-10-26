/* lptnspeclist.h -
 */

#ifndef _LPTNSPECLIST_H_
#define _LPTNSPECLIST_H_

#include "lptree/lptnspec.h"
#include "lptree/lptnspeclist-def.h"

G_BEGIN_DECLS



/* LptNSpecList:
 */
struct _LptNSpecList
{
  LPT_NSPEC_LIST_INSTANCE_HEADER;
};



/* LptNSpecListClass:
 */
struct _LptNSpecListClass
{
  LPT_NSPEC_LIST_CLASS_HEADER;
};



LptNSpec *lpt_nspec_list_new ( const gchar *name );



G_END_DECLS

#endif /* ifndef _LPTNSPECLIST_H_ */
