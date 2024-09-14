#ifndef fixlows_c
#define fixlows_c
#include <interface.c>

/////////////////////////////////////////////////////////////////////////////
//
//	Fixlows()
//
//	Brute-force hack to update all of the cached low keys for a node.
//	It should be possible to avoid this function when inserting or deleting,
//	but that can make the code cumbersome since child nodes may shift up,
//	down, or into sibling nodes when balancing.  The safest way to make
//	certain the cached keys are correct is to update all of them if there
//	is any possibility that one of them has changed.
//
void fixlows(ROOT)
{
	if (NULL != p_root) {
		// If each child node exists, we can grab the low1 key cached in
		// in it, since cached keys are always kept updated.
		// However, if this is a leaf, the child point will be NULL and the
		// correct look-up key will be stored in Ref.
		p_root->low1 = (NULL != p_root->p_child1) ? p_root->p_child1->low1 : p_root->p_child1->ref.key;
		p_root->low2 = (NULL != p_root->p_child2) ? p_root->p_child2->low1 : 0;
		p_root->low3 = (NULL != p_root->p_child3) ? p_root->p_child3->low1 : 0;
	}
}


#endif
