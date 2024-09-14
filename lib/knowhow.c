#ifndef knowhow_c
#define knowhow_c
#include <interface.c>
#include <public.c>
#include <protected.c>
#include <private.c>
#include <stdlib.h>
#include <insert.c>
#include <lookup.c>
#include <delete.c>

/////////////////////////////////////////////////////////////////////////////
//
//	KeyCountRec()
//
u_int32_t key_count_rec(ROOT)
{
	u_int32_t tally = 0;

	if (p_root->is_leaf) {
		tally += 1;
	}
	else {
		tally += key_count_rec(p_root->p_child1) + key_count_rec(p_root->p_child2);

		if (NULL != p_root->p_child3) {
			tally += key_count_rec(p_root->p_child3);
		}
	}

	return tally;
}


/////////////////////////////////////////////////////////////////////////////
//
//	KeyCount()
//
//	Recursively count the number of keys in the tree.
//
u_int32_t key_count(TREE)
{
	return key_count_rec(p_tree->data);
}


/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepthRec()
//
u_int32_t leaf_depth_rec(ROOT, u_int32_t depth, u_int32_t ary[], u_int32_t depthLimit)
{
	if (p_root->is_leaf) {
		ary[depth] += 1;
		return depth + 1;
	}

	if (depth >= depthLimit) {
		return depth;
	}

	u_int32_t d1 = leaf_depth_rec(p_root->p_child1, depth + 1, ary, depthLimit);
	u_int32_t d2 = leaf_depth_rec(p_root->p_child2, depth + 1, ary, depthLimit);

	u_int32_t maxDepth = Max(d1, d2);

	if (NULL != p_root->p_child3) {
		u_int32_t d3 = leaf_depth_rec(p_root->p_child3, depth + 1, ary, depthLimit);

		maxDepth = Max(maxDepth, d3);
	}

	return maxDepth;
}

/////////////////////////////////////////////////////////////////////////////
//
//	LeafDepth()
//
//	Recursively verify the depth of every leaf in the tree.  For a properly
//	formed 2-3 tree, every leaf should be at exactly the same depth, and
//	there should be one leaf for every key in the tree.
//
void leaf_depth(TREE)
{
	u_int32_t ary[64];
	for (u_int32_t i = 0; i < ArraySize(ary); ++i) {
		ary[i] = 0;
	}

	u_int32_t maxDepth = leaf_depth_rec(p_tree->data, 0, ary, ArraySize(ary));

	for (u_int32_t i = 0; i < maxDepth; ++i) {
		printf("%3d: %5d\n", i, ary[i]);
	}
}



/////////////////////////////////////////////////////////////////////////////
//
//	traverse_rec()
//
void traverse_rec(ROOT, u_int32_t *prev)
{
	if (p_root->is_leaf) {
		*prev = p_root->ref.key;

		printf("%4d", p_root->ref.key);

	}
	else {
		traverse_rec(p_root->p_child1, prev);
		traverse_rec(p_root->p_child2, prev);

		if (NULL != p_root->p_child3) {
			traverse_rec(p_root->p_child3, prev);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Traverse()
//
void traverse(TREE)
{
	if (NULL != p_tree) {
		u_int32_t prev = 0;
		traverse_rec(p_tree->data, &prev);
		printf("\n\n");
	}
}
/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheckRec()
//
void sanity_check_rec(ROOT)
{
	printf("%d %s\n",__LINE__,(NULL != p_root)?"ok":"error");

	if (p_root->is_leaf) {
		printf("%d %s\n",__LINE__,(p_root->low1 == p_root->ref.key)?"ok":"error");
		printf("%d %s\n",__LINE__,(0 == p_root->low2)?"ok":"error");
		printf("%d %s\n",__LINE__,(0 == p_root->low3)?"ok":"error");
		printf("%d %s\n",__LINE__,(NULL == p_root->p_child1)?"ok":"error");
		printf("%d %s\n",__LINE__,(NULL == p_root->p_child2)?"ok":"error");
		printf("%d %s\n",__LINE__,(NULL == p_root->p_child3)?"ok":"error");
	}
	else {
		printf("%d %s\n",__LINE__,(NULL != p_root->p_child1)?"ok":"error");
		printf("%d %s\n",__LINE__,(NULL != p_root->p_child2)?"ok":"error");

		printf("%d %s\n",__LINE__,(p_root->low1 == p_root->p_child1->low1)?"ok":"error");
		printf("%d %s\n",__LINE__,(p_root->low2 == p_root->p_child2->low1)?"ok":"error");

		printf("%d %s\n",__LINE__,(p_root->low1 < p_root->low2)?"ok":"error");

		if (NULL != p_root->p_child3) {
			printf("%d %s\n",__LINE__,(p_root->low3 == p_root->p_child3->low1)?"ok":"error");
			printf("%d %s\n",__LINE__,(p_root->low2 < p_root->low3)?"ok":"error");
		}

		if (p_root->p_child1->is_leaf) {
			printf("%d %s\n",__LINE__,(p_root->p_child2->is_leaf)?"ok":"error");

			if (NULL != p_root->p_child3) {
				printf("%d %s\n",__LINE__,(p_root->p_child3->is_leaf)?"ok":"error");
			}
		}

		sanity_check_rec(p_root->p_child1);
		sanity_check_rec(p_root->p_child2);

		if (NULL != p_root->p_child3) {
			sanity_check_rec(p_root->p_child3);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	SanityCheck()
//
//	This is a wrapper around logic to recursively scan through the tree and
//	verify that the tree is properly formed.
//
void sanity_check(TREE)
{
	if (NULL != p_tree->data) {
		sanity_check_rec(p_tree->data);
	}
}

#endif
