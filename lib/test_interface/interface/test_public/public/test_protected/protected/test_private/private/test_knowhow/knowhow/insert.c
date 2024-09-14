#ifndef insert_c
#define insert_c
#include <interface.c>
#include <stdlib.h>
#include <stdbool.h>
#include <new_node.c>

/////////////////////////////////////////////////////////////////////////////
//
//	InsertRec()
//
//	Recursively inserts an element into the tree.  It must first traverse
//	down to the leaf where the insertion should occur, insert the element,
//	then rebalance the tree while recursing back out again.
//
//	If a non-NULL pointer is returned, this indicates that p_root had to be
//	split, and that the returned node needs to be inserted into the parent
//	of p_root.
//
//	Note that one difference between this code and Aho is that their version
//	tracks the smallest key of pNew and returns it.  Their approach is
//	mildly cumbersome (and more an artifact of the Pascal-like format of
//	their pseudocode), and requires passing more data around.  Due to the
//	logic of insertion, after a recursive call the lows of p_child will be
//	correct, so we can directly reference p_root's children to update its
//	lows and keep everything in sync.
//
node_t* insert_rec(ROOT, void_ref_t ref)
{
	node_t *pNew = NULL;

	// The first half of this function is dedicated to descending down the
	// tree until it reaches the leaf node where the insertion should occur.
	//
	if (false == p_root->is_leaf) {

		//
		// Determine which of the 2 or 3 children should contain the element
		// that is being inserted.
		//

		u_int32_t childIndex = Child_3;
		node_t *p_child = p_root->p_child3;

		if (ref.key < p_root->low2) {
			childIndex = Child_1;
			p_child     = p_root->p_child1;
		}
		else if ((NULL == p_root->p_child3) || (ref.key < p_root->low3)) {
			childIndex = Child_2;
			p_child     = p_root->p_child2;
		}

		// Recurse down the appropriate child.
		node_t *p_back = insert_rec(p_child, ref);

		// If a non-NULL pointer was returned, we need to insert it into
		// p_root, which may require splitting p_root.
		if (NULL != p_back) {

			// If p_root only contains two children, we can insert p_back
			// into p_root without requiring a split.
			//
			// Note that due to the logic of InsertRec(), p_back should be
			// the second child of p_root since it is guaranteed to contain a
			// key larger than p_child.  The recursive call to InsertRec()
			// will swap the contents of p_back and p_child to guarantee that
			// this is true.
			//
			if (NULL == p_root->p_child3) {
				p_root->is_leaf = false;

				// If the insertion was to the second child, we can drop
				// p_back in as the third child without any extra effort.
				if (Child_2 == childIndex) {
					p_root->p_child3 = p_back;
					p_root->low3    = p_back->low1;
				}

				// However, if the insertion was to the first child, p_back
				// needs to be inserted after it -- again making it the
				// second child.  This requires shifting the current second
				// child over to be the third child, then dropping p_back in
				// as the third child.
				else {
					p_root->p_child3 = p_root->p_child2;
					p_root->low3    = p_root->p_child2->low1;
					p_root->p_child2 = p_back;
					p_root->low2    = p_back->low1;

					// Note that we must fix up the cached low key for the
					// first child, since the recursive call may have swapped
					// the contents of p_child and p_back before returning,
					// meaning that p_root's cached key is now invalid.
					p_root->low1    = p_root->p_child1->low1;
				}
			}

			// Otherwise p_root is a 3-node, so we need to split it into a
			// pair of 2-nodes.
			else {
				pNew = new_node();

				pNew->is_leaf = false;

				// p_back is to the right of child 3, so child 3 and p_back
				// become the children of the new node.
				if (Child_3 == childIndex) {
					pNew->p_child1  = p_root->p_child3;
					pNew->low1     = p_root->p_child3->low1;
					pNew->p_child2  = p_back;
					pNew->low2     = p_back->low1;
				}

				// p_back comes between child 2 and child 3, so p_back and
				// child 3 become the children of pNew -- the difference
				// from the previous condition being the order in which
				// they are stored in pNew
				else if (Child_2 == childIndex) {
					pNew->p_child1  = p_back;
					pNew->low1     = p_back->low1;
					pNew->p_child2  = p_root->p_child3;
					pNew->low2     = p_root->p_child3->low1;
				}

				// Otherwise p_back comes between child 1 and child 2, so
				// we have to move child 2 and child 3 into pNew, then
				// make p_back the second child of p_root
				else {
					pNew->p_child1  = p_root->p_child2;
					pNew->low1     = p_root->p_child2->low1;
					pNew->p_child2  = p_root->p_child3;
					pNew->low2     = p_root->p_child3->low1;
					p_root->p_child2 = p_back;
					p_root->low2    = p_back->low1;

					// Again, the recursive call to InsertRec() may have
					// swapped p_child and p_back before returning, so we
					// have to fix up the cached low key for child 1.
					p_root->low1    = p_root->p_child1->low1;
				}

				p_root->p_child3 = NULL;
				p_root->low3    = 0;
			}
		}

		// Otherwise the child node was changed, but p_root does not need to
		// be split after the insert.  However, we still need to update the
		// correct low key for the node to guarantee that it is being
		// cached correctly.
		//
		else if (Child_1 == childIndex) {
			p_root->low1 = p_root->p_child1->low1;
		}
		else if (Child_2 == childIndex) {
			p_root->low2 = p_root->p_child2->low1;
		}
		else {
			p_root->low3 = p_root->p_child3->low1;
		}
	}

	// Once the code hits a leaf, this block of code will create a new node
	// to contain the data (or return NULL if nothing was inserted).
	else {

		// Leaf nodes should never be empty unless we're dealing with a
		// degenerate tree (possibly resulting from deleting all elements,
		// then attempting to insert a new element).
		//
		// But we do need to detect and prevent a key from being inserted
		// into the tree multiple times -- this implementation of 2-3 trees
		// does not support duplicate keys.
		//
		if ((NULL != p_root->ref.p_context) && (ref.key != p_root->ref.key)) {

			pNew = new_node();

			// Arrange p_root and pNew so that p_root contains the smaller
			// of the two keys.  We'll return pNew so that the caller can
			// insert the new node into the parent (or split the parent if
			// necessary).
			if (p_root->ref.key < ref.key) {
				pNew->ref   = ref;
				pNew->low1  = ref.key;
			}
			else {
				pNew->ref   = p_root->ref;
				pNew->low1  = p_root->ref.key;
				p_root->ref  = ref;
				p_root->low1 = ref.key;
			}
		}

		// Special case for when the leaf node has nothing in it.
		// This is needed for inserting into a tree that contains a
		// single, empty node -- in practice, this should never happen.
		//
		// Otherwise, this condition will handle replacing the old value
		// when the key is already in the tree: no allocation is required,
		// just overwrite the old value.
		//
		else {
			p_root->ref  = ref;
			p_root->low1 = ref.key;

			// NOTE: This leaves pNew set to NULL, so the caller knows that
			// nothing needs to be updated.
		}
	}

	// If a new node was created, we need to pass it to the caller so it can
	// add it to the parent node (possibly requiring the parent to be split).
	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Insert()
//
//	Insertions are moderately complex, since various special cases must be
//	addressed to rebalance the tree following the insertion.  Insertion is
//	a recursive operation (though it could be made iterative if a stack is
//	used), so most of the complexity resides in the recursive sub-function.
//
void insert(TREE, void_ref_t ref)
{
	// In the normal case, the tree is not empty.
	if (NULL != p_tree->data) {

		// Normally all of the insertion work will be done recursively by
		// InsertRec().  However, insertion may cause nodes to be split,
		// which can propagate up the tree to the root.
		//
		// If this returns a non-NULL pointer, then the current root has
		// been split, and p_back is a sibling for the current root.
		//
		node_t *p_back = insert_rec(p_tree->data, ref);

		// Create a new root, with p_tree->data and p_back as its children.
		//
		// InsertRec() will have swapped the contents of the current p_tree->data
		// with p_root to assure that p_tree->data < p_root.  Therefore this
		// code can safely make p_tree->data the first child and p_root the second
		// child of the new root node.
		//
		// Note that this increases the depth of the tree by one.
		//
		if (NULL != p_back) {
			node_t *pNew = new_node();
			pNew->is_leaf  = false;
			pNew->p_child1 = p_tree->data;
			pNew->p_child2 = p_back;
			pNew->low1    = p_tree->data->low1;
			pNew->low2    = p_back->low1;
			p_tree->data       = pNew;
		}
	}

	// Special case for inserting into an empty tree.
	else {
		p_tree->data = new_node();
		p_tree->data->is_leaf = true;
		p_tree->data->ref    = ref;
		p_tree->data->low1   = ref.key;
	}
}



#endif
