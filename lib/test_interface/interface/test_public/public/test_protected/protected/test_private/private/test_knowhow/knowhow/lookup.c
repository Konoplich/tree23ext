#ifndef lookup_c
#define lookup_c
#include <interface.c>
/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Iteratively traverses the tree looking for the given search key.
//	Returns the context pointer if the key exists within the tree.
//	Returns NULL if the key cannot be found.
//
void* lookup(TREE, const u_int32_t key)
{
	if (NULL == p_tree->data) {
		return NULL;
	}

	node_t *pScan = p_tree->data;

	while (NULL != pScan) {
		// Data values are only stored in leaf nodes.  Once we hit a leaf,
		// we can check if this is the leaf that contains the given key.
		if (pScan->is_leaf) {
			if (key == pScan->ref.key) {
				return pScan->ref.p_context;
			}
		}

		// The third child may not exist, so only traverse that way when the
		// node is present.
		if ((NULL != pScan->p_child3) && (key >= pScan->low3)) {
			pScan = pScan->p_child3;
		}
		else if (key >= pScan->low2) {
			pScan = pScan->p_child2;
		}
		else {
			pScan = pScan->p_child1;
		}
	}

	return NULL;
}


#endif
