#ifndef child_count_c
#define child_count_c
#include <interface.c>

/////////////////////////////////////////////////////////////////////////////
//
//	ChildCount()
//
u_int32_t child_count(ROOT)
{
	if (NULL != p_root) {
		if (NULL != p_root->p_child3) {
			return 3;
		}

		if (NULL != p_root->p_child2) {
			return 2;
		}

		if (NULL != p_root->p_child1) {
			return 1;
		}
	}

	return 0;
}


#endif
