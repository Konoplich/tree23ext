#ifndef delete_c
#define delete_c
#include <interface.c>
#include <stdio.h>
#include <stdbool.h>
#include <shift_children_down.c>
#include <stdlib.h>
#include <child_count.c>
#include <fixlows.c>
/////////////////////////////////////////////////////////////////////////////
//
//	DeleteRec()
//
//	Returns true if merging caused p_root to end up with a single child,
//	which requires the caller to merge p_root with a sibling (or delete it
//	if p_root is the root of the tree).
//
bool delete_rec(ROOT, const u_int32_t key)
{
	// If we reach a leaf that does not contain the key for deletion,
	// then we know that the key is not in the tree.  No deletion needs
	// to be returned, nor will the caller need to perform any fix-up
	// operations.  Return false so the caller knows no further work is
	// required.
	if (p_root->is_leaf && (key != p_root->ref.key)) {
		return false;
	}

	// We cannot be in a leaf if the key has not yet been found.  That case
	// is handled in the parent node of the leaf, since all of the fix-up
	// work needs to be done in that node.
	if(false != p_root->is_leaf) printf("assert %d\n", __LINE__);


	///////////////////////////////
	//  Deletion of leaf nodes.  //
	///////////////////////////////

	// Special case: Child 1 is a leaf that contains the key to delete.
	if ((NULL != p_root->p_child1) &&
		(p_root->p_child1->is_leaf) &&
		(key == p_root->p_child1->ref.key))
	{
		// Delete the first child, then shift the two remaining children down.
		// Since this is not a leaf node, it contains at least two children.
		free(p_root->p_child1);
		shift_children_down(p_root);
		// If the deletion resulted in this node having only one child,
		// return true so fix-up operations can be performed on the parent.
		return (NULL == p_root->p_child2);
	}

	// Special case: Child 2 is a leaf that contains the key to delete.
	if ((NULL != p_root->p_child2) &&
		(p_root->p_child2->is_leaf) &&
		(key == p_root->p_child2->ref.key))
	{
		// Delete the second child.
		free(p_root->p_child2);

		// Shift the third child down to replace the second.
		p_root->p_child2 = p_root->p_child3;
		p_root->p_child3 = NULL;

		// Third child is now a NULL reference.
		p_root->low2 = p_root->low3;
		p_root->low3 = 0;

		// If there is no second child, return true so fix-up operations can
		// be applied to the parent node.
		return (NULL == p_root->p_child2);
	}

	// Special case: Child 3 is a leaf that contains the key to delete.
	if ((NULL != p_root->p_child3) &&
		(p_root->p_child3->is_leaf) &&
		(key == p_root->p_child3->ref.key))
	{
		// This is the easiest case.  Just delete the third node and return
		// false.  No fix-up operation will be required since we just turned
		// a 3-node into a 2-node.
		free(p_root->p_child3);

		p_root->p_child3 = NULL;
		p_root->low3    = 0;

		return false;
	}


	//////////////////////////
	//  Fix-up operations.  //
	//////////////////////////

	bool result = false;

	// The key to delete is contained in the third child.  The recursive
	// call to DeleteRec() will remove the target node from the tree.
	if ((NULL != p_root->p_child3) && (key >= p_root->low3)) {
		bool doFixup = delete_rec(p_root->p_child3, key);

		// If p_child3 ended up with only one child, we need to apply a fix-up
		// operation so it will have two children.
		if (doFixup) {
			node_t *three = p_root->p_child3;
			node_t *two   = p_root->p_child2;

			// The simple case is when child 2 has three children.  We just
			// need to move child 2's third child over to child 3.  No
			// deletion is required in this case.
	if (3 == child_count(two)) {
				// Three's child1 becomes child2.
				three->p_child3 = three->p_child2;
				three->p_child2 = three->p_child1;

				// Two's child3 becomes three's child 1.
				three->p_child1 = two->p_child3;
				two->p_child3   = NULL;

				// Directly fix up the low keys.
				three->low3    = three->low2;
				three->low2    = three->low1;
				three->low1    = two->low3;
				two->low3      = 0;
			}

			// Otherwise child two only has 2 children, so we need to merge.
			else {
				// Three's only child become two's third child.
				two->p_child3   = three->p_child1;
				two->low3      = three->low1;

				// Clear the pointer before we delete the node.
				three->p_child1 = NULL;
				free(three);

				// Fix this node, since it no longer has a third child.
				p_root->p_child3 = NULL;
				p_root->low3    = 0;
			}
		}

		// Repair the low key references to the children.  This needs to
		// always be performed, even if no fix-up was required for this node.
		// If any fix-up was applied to the child node, its low keys may
		// have changed, so we must propagate the updated low keys up the
		// tree.
		fixlows(p_root);

		// We can always resolve merging or swapping at this level, so the
		// caller does not need to do any more repair work.
		return false;
	}


	// The node to delete is contained within the second child.
	if ((NULL != p_root->p_child2) && (key >= p_root->low2)) {
		bool doFixup = delete_rec(p_root->p_child2, key);

		// If child 2 ended up with only one child, we need to either move a
		// second node into child 2, or delete child 2.
		if (doFixup) {
			node_t *two   = p_root->p_child2;
			node_t *one   = p_root->p_child1;
			node_t *three = p_root->p_child3;

			// If child 1 contains three nodes, we can move one of those
			// nodes over to child 2.
			if (3 == child_count(one)) {
				// Two's child 1 becomes its child 2.
				two->p_child2 = two->p_child1;
				two->low2    = two->low1;

				// One's child 3 becomes two's child 1.
				two->p_child1 = one->p_child3;
				two->low1    = one->low3;

				// One no longer has a child 3.
				one->p_child3 = NULL;
				one->low3    = 0;
			}

			// If child 3 has three nodes, we can move one of those nodes
			// into child 2.
			//
			// Note that ChildCount() returns 0 if three does not exist,
			// so we don't need extra testing logic here.
			//
			else if (3 == child_count(three)) {
				// Three's child 1 becomes two's child 2.
				two->p_child2 = three->p_child1;
				two->low2    = three->low1;

				// Three's other children shift down one to fill up the hole.
				three->p_child1 = three->p_child2;
				three->p_child2 = three->p_child3;
				three->p_child3 = NULL;
				three->low1    = three->low2;
				three->low2    = three->low3;
				three->low3    = 0;
			}

			// Otherwise child 1 only has two nodes (and child 3 either has
			// only two children, or it does not exist at all).
			else {
				// Two's only child becomes one's child 3.
				one->p_child3 = two->p_child1;
				one->low3    = two->low1;

				// Clear the pointer so we can delete the node.
				two->p_child1 = NULL;
				free(two);

				// Child 3 may or may not exist.  It is safe to copy it over
				// to replace child 2, since at this stage we just need to
				// fix up the pointers in this node.
				p_root->p_child2 = p_root->p_child3;
				p_root->low2    = p_root->low3;
				p_root->p_child3 = NULL;
				p_root->low3    = 0;

				// However, if there was no Child 3 before, then we've
				// created a degenerate node.  Return true if there is no
				// Child 2 so that the caller knows it has fix-up work to do.
				result = (NULL == p_root->p_child2);
			}
		}

		// Repair the low key references to the children.  This needs to
		// always be performed, even if no fix-up was required for this node.
		// If any fix-up was applied to the child node, its low keys may
		// have changed, so we must propagate the updated low keys up the
		// tree.
		fixlows(p_root);

		return result;
	}


	// Otherwise the node to delete exists under the first child.  Let the
	// recursive logic handle the deletion.
	bool doFixup = delete_rec(p_root->p_child1, key);

	// If child 1 ended up with only one child, we need to fix it up so it
	// has two children, or delete it.
	if (doFixup) {
		node_t *one = p_root->p_child1;
		node_t *two = p_root->p_child2;

		// If child 2 has three children, then we can move one of them over
		// to fill in child 1.
		if (3 == child_count(two)) {
			// Child 2's first child is moved over to Child 1.
			one->p_child2 = two->p_child1;
			one->low2    = two->low1;

			// Shift Child 2's other children down to fill up the hole.
			two->p_child1 = two->p_child2;
			two->p_child2 = two->p_child3;
			two->p_child3 = NULL;

			two->low1 = two->low2;
			two->low2 = two->low3;
			two->low3 = 0;
		}

		// Otherwise Child 2 only has two children, so we have to merge
		// Child 1 into child 2.
		else {
			// Shift Child 2's children up one to open up a hole.
			two->p_child3 = two->p_child2;
			two->low3    = two->low2;
			two->p_child2 = two->p_child1;
			two->low2    = two->low1;

			// Child 1's only child is moved over to child 2.
			two->p_child1 = one->p_child1;
			two->low1    = one->low1;

			// Clear the pointer so we can delete this node.
			one->p_child1 = NULL;
			free(one);
			// We've now made a hole in the current node, so we need to
			// shift it's children down one to fill in the hole.
			shift_children_down(p_root);

			// If the current node has ended up with only one child, we need
			// to return true so that the caller knows it needs to perform
			// fix-up operations on the parent node.
			result = (NULL == p_root->p_child2);
		}
	}

	// Repair the low key references to the children.  This needs to always
	// be performed, even if no fix-up was required for this node.  If any
	// fix-up was applied to the child node, its low keys may have changed,
	// so we must propagate the updated low keys up the tree.
	fixlows(p_root);

	return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
//	This function is called to delete a node from the tree.  Most of the
//	work is handled recursively by DeleteRec().  This function only needs to
//	trap a couple of special cases, and to handle deleting the root when
//	merging nodes.
//
void delete(TREE, const u_int32_t key)
{
	// Special case for an empty tree.
	if (NULL == p_tree->data) {
		return;
	}

	// Special case for deleting the last node in the tree.
	if (p_tree->data->is_leaf && (key == p_tree->data->ref.key)) {
		free(p_tree->data);
		p_tree->data = NULL;
		return;
	}

	// DeleteRec() will do all of the work.  The only special effort needed
	// is when p_tree->data ends up with a single child (that is not a leaf).
	// If this happens, we need to delete the current root node and make its
	// child the new root of the tree (which causes the depth of the tree to
	// decrease by one level, and is the only way in which the depth of the
	// leaves will change due to deletion).
	//
	if (delete_rec(p_tree->data, key)) {
		node_t *pTemp = p_tree->data;

		p_tree->data = p_tree->data->p_child1;

		pTemp->p_child1 = NULL;

		free(pTemp);
	}
}

#endif
