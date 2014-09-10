/* lptnspecdir.h -
 */

#ifndef _LPTNSPECDIR_H_
#define _LPTNSPECDIR_H_

#include "lptree/lptnspec.h"
#include "lptree/lptnspecdir-def.h"



/* LptNSpecDir:
 */
struct _LptNSpecDir
{
  LPT_NSPEC_DIR_INSTANCE_HEADER;
};



/* LptNSpecDirClass:
 */
struct _LptNSpecDirClass
{
  LPT_NSPEC_DIR_CLASS_HEADER;
};



LptNSpec *lpt_nspec_dir_new ( const gchar *name );



#endif
