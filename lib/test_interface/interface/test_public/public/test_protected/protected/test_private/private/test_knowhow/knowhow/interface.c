#ifndef interface_c
#define interface_c
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

#define TREE tree23ext_t *p_tree
#define PTREE tree23ext_t **p_tree
#define ROOT node_t *p_root
#define SafeDelete(x)		{ if (NULL != (x)) { free(x);     (x) = NULL; } }

//#define DEBUG

#ifdef DEBUG
    #define p(...) printf("line %d ", __LINE__); printf(__VA_ARGS__);
#else
    #define p /**/
#endif

#define ArraySize(x) (sizeof(x) / (sizeof((x)[0])))
#define Max(x, y) ((x)>(y)) ? (x) : (y)

#define Child_1		0
#define Child_2		1
#define Child_3		2

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

	void_ref_t ref;
	u_int32_t low1;
    u_int32_t low2;
    u_int32_t low3;
    bool is_leaf;
} node_t;

typedef struct _tree23ext
{
    node_t *data;
} tree23ext_t;

void insert(TREE, void_ref_t ref);
void delete(TREE, const u_int32_t key);
void* lookup(TREE, const u_int32_t key);

#endif
