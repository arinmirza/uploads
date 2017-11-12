/* gcc -Wall -o tree blatt3-rahmen.c --std=c99 */
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct node {
    struct node *left;
    struct node *right;
    int val;
};


struct node *new_node(int val)
{
    struct node * new = (struct node*) malloc(sizeof(struct node));
    new -> left  = NULL;
    new -> right = NULL;
    new -> val   = val; 
}

struct node *insert_rec(struct node *node,
                        int val)
{
    if(node == NULL)
        return new_node(val);
    if(val < node -> val)
        if(node -> left == NULL) {
            node -> left = new_node(val);
            return node -> left;
        }
        else
            return insert_rec(node -> left, val);
    else
        if(node -> right == NULL) {
            node -> right = new_node(val);
            return node -> right;
        }
        else
            return insert_rec(node -> right, val);
}

struct node *make_random_tree(int num_nodes)
{
    srand(time(0));
    struct node * root = new_node(rand() % 100);
    printf("Adding value %d.\n", root -> val);
    num_nodes--;
    while(num_nodes > 0) {
        int val = rand() % 100;
        insert_rec(root, val);
        printf("Adding value %d.\n", val);
        num_nodes--;
    }
    return root;
}

void iterate_in_order(struct node *node,
                      void (*f)(struct node *n))
{
    if(node == NULL)
        return;
    iterate_in_order(node -> left, f);
    f(node);
    iterate_in_order(node -> right, f);
}

void iterate_pre_order(struct node *node,
                      void (*f)(struct node *n))
{
    if(node == NULL)
        return;
    f(node);
    iterate_pre_order(node -> left, f);
    iterate_pre_order(node -> right, f);
}


void iterate_post_order(struct node *node,
                        void (*f)(struct node *n))
{
    if(node == NULL)
        return;
    iterate_post_order(node -> left, f);
    iterate_post_order(node -> right, f);
    f(node);
}

/* Print the node's value */
void print_val(struct node *node)
{
   printf("Value of node %p: %d\n", node, node -> val);
}

/* Add one to the node's value */
void plusone(struct node *node)
{
    node -> val = node -> val + 1;
}

/* N.B.: -x invertiert die BST-Invariante
 *  * (rechter Teilbaum jetzt groesser als linker) */
void negate(struct node *node)
{
    node -> val = -(node -> val);
}

/* N.B.: Nicht-monotone Funktionen zerstoeren
 * die Struktur des Suchbaumes */
void magic(struct node *node)
{
    int x = node -> val;
    node -> val = (100-x)*x - 900;
}

/* Free only one node */
void free_node(struct node *node)
{
    free(node);
}

void delete_tree(struct node *node) {
    /* Jeder Teilbaum muss geloescht werden bevor der Wurzel
       geloescht werden kann. Ansonsten wird die Adresse
       dieses Teilbaum verloren und die Knoten darin koennen
       nie geloescht werden.
    */
    iterate_post_order(node, free_node);
}

int main(int argc, char *argv[])
{

    struct node *root = NULL;

    root = make_random_tree(5);

    printf("Printing in order:\n");
    iterate_in_order(root, print_val);
    printf("\n");

    printf("Printing in pre-order:\n");
    iterate_pre_order(root, print_val);
    printf("\n");

    printf("Printing in post-order:\n");
    iterate_post_order(root, print_val);
    printf("\n");

    /* Apply the manipulation functions */
    printf("Original:\n");
    iterate_pre_order(root, print_val);
    printf("plusone:\n");
    iterate_in_order(root, plusone);
    iterate_pre_order(root, print_val);
    printf("negate:\n");
    iterate_in_order(root, negate);
    iterate_pre_order(root, print_val);
    printf("magic:\n");
    iterate_in_order(root, magic);
    iterate_pre_order(root, print_val);

    printf("Printing in order (squared):\n");
    iterate_in_order(root, print_val);
    printf("\n");

    /* Free each node*/
    delete_tree(root);
    //iterate_post_order(root, free_node);

    return EXIT_SUCCESS;
}
