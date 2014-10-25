/* lptcheck.h -
 */

#ifndef _LPTCHECK_H_
#define _LPTCHECK_H_

#include "lptree/lpttree.h"
#include "lptree/lptnode.h"
#include <los/lostest.h>

#define CHECK_NODE_DIR(n) do {                          \
    LptNode *_check_node = (n);                         \
    PIF_CHECK(_check_node);                             \
    PIF_CHECK(_check_node->nspec);                      \
    PIF_CHECK(LPT_IS_NSPEC_DIR(_check_node->nspec));    \
  } while (0)

#define CHECK_NODE_INT(n, val) do {                     \
    LptNode *_check_node = (n);                         \
    PIF_CHECK(_check_node);                             \
    PIF_CHECK(_check_node->nspec);                      \
    PIF_CHECK(LPT_IS_NSPEC_INT(_check_node->nspec));    \
    LOS_CHECK_INT                                       \
      (L_TRASH_OBJECT(lpt_node_get_value(_check_node)), \
       (val));                                          \
  } while (0)

struct check_node_tree_data
{
  LptTree *tree;
};

static void _check_node_tree ( LptNode *node,
                               gpointer data_ )
{
  struct check_node_tree_data *data = data_;
  PIF_CHECK(node);
  PIF_CHECK_EQ_PTR(lpt_node_get_tree(node), data->tree);
  /* traverse */
  lpt_node_foreach(node, _check_node_tree, data);
}

/* sanity check for a whole nod subtree */
static void check_node_tree ( LptNode *node,
                              LptTree *tree )
{
  struct check_node_tree_data data;
  data.tree = tree;
  _check_node_tree(node, &data);
}

#endif
