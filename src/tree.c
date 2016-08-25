#include "tree.h"
#include "extends.h"

#include <stdio.h>
#include <stdlib.h>

const char ERROR=1,
           OK=0;


Tree* tree_init(int(*find_comparision)(char *s1, char *s2),
                int(*add_comparision)(char *s1, char *s2),
                void (*free_data)(char *data))
{
    Tree *tree=new(Tree);

    tree->root             = 0;
    tree->length           = 0;
    tree->find_comparision = find_comparision;
    tree->add_comparision  = add_comparision;
    tree->free_data        = free_data;

    return tree;
}


static unsigned char height(TreeNode *p)
{
    if(p)
        return p->height;

    return 0;
}


static int bfactor(TreeNode *p)
{
    return height(p->right)-height(p->left);
}


static void fixheight(TreeNode *p)
{
    unsigned char hl=height(p->left);
    unsigned char hr=height(p->right);

    if(hl>hr)
        p->height=hl+1;
    else
        p->height=hr+1;
}


static TreeNode* rotateright(TreeNode *p)
{
    TreeNode *q=p->left;

    p->left=q->right;
    q->right=p;
    fixheight(p);
    fixheight(q);

    return q;
}


static TreeNode* rotateleft(TreeNode *q)
{
    TreeNode *p=q->right;

    q->right=p->left;
    p->left=q;
    fixheight(q);
    fixheight(p);

    return p;
}


static TreeNode* balance(TreeNode *p)
{
    fixheight(p);

    if(bfactor(p)==2)
    {
        if(bfactor(p->right)<0)
            p->right=rotateright(p->right);

        return rotateleft(p);
    }

    if(bfactor(p)==-2)
    {
        if(bfactor(p->left)>0)
            p->left=rotateleft(p->left);

        return rotateright(p);
    }

    return p;
}


static TreeNode *tree_add_node(Tree *tree)
{
    TreeNode *node=tree->current_node;
    int cmp;

    if(node)
    {
        cmp=tree->add_comparision(tree->add_data, node->data);

        if(cmp<0)
        {
            tree->current_node=node->left;
            node->left=tree_add_node(tree);
        }
        else
        {
            tree->current_node=node->right;
            node->right=tree_add_node(tree);
        }
    }
    else
    {
        node=malloc(sizeof(TreeNode));
        node->data=tree->add_data;
        node->left=0;
        node->right=0;
        node->height=1;
    }

    return balance(node);
}


void tree_add(Tree *tree, char *data)
{
    tree->add_data=data;

    tree->current_node=tree->root;
    tree->root=tree_add_node(tree);

    tree->length++;
}


char* tree_find(Tree *tree, char *data)
{
    TreeNode  *i=tree->root;
    int      (*comparision)(char *s1, char *s2)=tree->find_comparision;
    int        cmp;

    while(i)
    {
        cmp=comparision(data, i->data);

        if(!cmp)
            return i->data;
        else if(cmp<0)
            i=i->left;
        else
            i=i->right;
    }

    return 0;
}


static void (*tree_print_func)(char *data);
static int l=0;

static void tree_print_node(TreeNode *node)
{
    int i;

    if(node!=0)
    {
        l++;
        tree_print_node(node->left);
        l--;

        //for(i=0; i<l; i++)
          //  printf("   ");
        tree_print_func(node->data);

        l++;
        tree_print_node(node->right);
        l--;
    }
}


void tree_print(Tree *tree, void (*print_data)(char *data))
{
    tree_print_func=print_data;
    tree_print_node(tree->root);
}


static void tree_free_node(Tree *tree)
{
    TreeNode *node=tree->current_node;

    if(node!=0)
    {
        tree->free_data(node->data);

        tree->current_node=node->left;
        tree_free_node(tree);

        tree->current_node=node->right;
        tree_free_node(tree);

        free(node);
    }
}


void tree_free(Tree *tree)
{
    tree->current_node=tree->root;
    tree_free_node(tree->root);
    free(tree);
}
