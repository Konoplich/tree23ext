#ifndef shift_children_down_c
#define shift_children_down_c
#include <interface.c>

/////////////////////////////////////////////////////////////////////////////
//
//	ShiftChildrenDown()
//
//	Shifts all of the child nodes down the sequence of pointers (or to the
//	left in the tree, depending on how you look at the operation).  This is
//	useful when removing the first child due to balancing (when the first
//	child is rotated up a level or to a sibling node).
//
//	When this is done, the third child will end up as a NULL pointer.
//
void shift_children_down(ROOT)
{
	p_root->p_child1 = p_root->p_child2;
	p_root->p_child2 = p_root->p_child3;
	p_root->p_child3 = NULL;

	p_root->low1 = p_root->low2;
	p_root->low2 = p_root->low3;
	p_root->low3 = 0;
}


#endif
