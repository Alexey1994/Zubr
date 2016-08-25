#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

typedef struct
{
    char *data;
    struct TreeNode *left, *right;
    unsigned char height;
}TreeNode;

typedef struct
{
    TreeNode *root;
    int       length;

    char    (*add_comparision)(char *, char *);
    char    (*find_comparision)(char *, char *);
    void    (*free_data)(char *data);
    char     *add_data;
    TreeNode *current_node;
}Tree;

Tree* tree_init(int(*find_comparision)(char *s1, char *s2),
                int(*add_comparision)(char *s1, char *s2),
                void (*free_data)(char *data));

void tree_free(Tree *tree);

void tree_add(Tree *tree, char *data);
char* tree_find(Tree *tree, char *data);

void tree_print(Tree *tree, void (*print_data)(char *data));

#endif // TREE_H_INCLUDED
