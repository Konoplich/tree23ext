#ifndef lib
#define lib

#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

#define TREE tree23int_t *p_tree
#define PTREE tree23int_t **p_tree
#define ROOT node_t *p_root
#define SafeDelete(x)		{ if (NULL != (x)) { free_node(x);     (x) = NULL; } }

typedef struct _void_ref
{
	u_int32_t  key;
	void* p_context;
} void_ref_t;

typedef struct _node
{
	struct _node *p_child1;
	struct _node *p_child2;
	struct _node *p_child3;

	void_ref_t ref_lo;
	void_ref_t ref_hi;
} node_t;

typedef struct _tree23int
{
    node_t *data;
} tree23int_t;

void insert(TREE, void_ref_t ref);
void delete(TREE, const u_int32_t key);
void* lookup(TREE, const u_int32_t key);

bool new(PTREE);
void destructor(PTREE);
void print(TREE);
void leaf_depth(TREE);
void traverse(TREE);
u_int32_t key_count(TREE);
void sanity_check(TREE);
#endif
