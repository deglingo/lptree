/* lpt_path.c -
 */

#include "lptree/private.h"
#include "lptree/lptpath.h"



/* lpt_path_next_key:
 */
LObject *lpt_path_next_key ( const gchar **path )
{
  const gchar *pstart, *pend;
  /* skip leading slashes */
  for (pstart = *path; (*pstart) == '/'; pstart++);
  for (pend = pstart; (*pend) && (*pend) != '/'; pend++);
  *path = pend;
  if (pend == pstart)
    return NULL;
  else
    return L_OBJECT(l_string_new_len(pstart, pend - pstart));
}
