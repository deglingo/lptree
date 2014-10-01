/* lptcheck.h -
 */

#ifndef _LPTCHECK_H_
#define _LPTCHECK_H_

#include "lptree/lpttree.h"
#include "lptree/lptnode.h"

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
