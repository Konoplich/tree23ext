#ifndef public_c
#define public_c
#include <interface.c>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

bool new(PTREE)
{
    *p_tree = malloc(sizeof(tree23ext_t));
    if(NULL !=*p_tree)
    {
        memset(*p_tree, 0, sizeof(tree23ext_t));
        return true;
    }
    return false;
}

void free_node(ROOT)
{
    p("free node %p\n", p_root);
	if (NULL != p_root) {
		if (NULL != p_root->p_child1) {
			free_node(p_root->p_child1);
		}
		if (NULL != p_root->p_child2) {
			free_node(p_root->p_child2);
		}
		if (NULL != p_root->p_child3) {
			free_node(p_root->p_child3);
		}

		SafeDelete(p_root);
	}
}

void destructor(PTREE)
{
    p("destructor enter\n");
    p("destructor: %p\n", (*p_tree));
    free_node((*p_tree)->data);
    free(*p_tree);
    p_tree = NULL;
}

void print_rec(ROOT, char* end, char* start)
{
    char *pr = (end-3<start)?"...":end-3;
    if(p_root->is_leaf)
    {
        printf("%s leaf %p, [%d-%s]\n", pr,  p_root, p_root->ref.key, (char*)p_root->ref.p_context);
    }
    else{
        printf("%s child1 %p\n", pr, p_root->p_child1);
        print_rec(p_root->p_child1, pr, start);
        printf("%s child2 %p\n", pr, p_root->p_child2);
        print_rec(p_root->p_child2, pr, start);
        if(NULL != p_root->p_child3)
        {
            printf("%s child3 %p\n", pr, p_root->p_child3);
            print_rec(p_root->p_child3, pr, start);
        }
    }
}

void print(TREE)
{
    static char prefix[] = "-----------------------------------------------------------------------";
    if(NULL != p_tree && NULL !=p_tree->data)
	    print_rec(p_tree->data, &prefix[sizeof(prefix)-2], &prefix[0]);
}

#endif
