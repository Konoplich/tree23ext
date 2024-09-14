#ifndef new_node_c
#define new_node_c
#include <interface.c>
#include <stdlib.h>

node_t *new_node(void)
{
	node_t *p_new = malloc(sizeof(node_t));

	p_new->ref.key      = 0;
	p_new->ref.p_context = NULL;
	p_new->low1 = 0;
	p_new->low2        = 0;
	p_new->low3        = 0;
	p_new->is_leaf        = true;
    p_new->p_child1 = NULL;
    p_new->p_child2 = NULL;
    p_new->p_child3 = NULL;
	return p_new;
}

#endif
