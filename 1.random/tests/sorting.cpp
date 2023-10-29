#include <iostream>

typedef struct Node
{
    int value;
    struct Node *left;
    struct Node *right;
} Node;

Node *newNode(int item)
{
    Node *tmp = (Node *)calloc(1, sizeof(Node));
    tmp->value = item;
    return tmp;
}

void printvalues(Node *root)
{
    if (root != NULL)
    {
        printvalues(root->left);
        std::cout << root->value << " " << std::endl;
        printvalues(root->right);
    }
}

Node *insert(Node *node, int value)
{
    if (node == NULL)
        return newNode(value);
    if (value < node->value)
        node->left = insert(node->left, value);
    else if (value > node->value)
        node->right = insert(node->right, value);
    return node;
}

int main()
{
    int arr[] = {5, 4, 7, 2, 11};
    int n = sizeof(arr) / sizeof(arr[0]);
    Node *root = NULL;

    root = insert(root, arr[0]);
    for (int i = 1; i < n; i++)
        root = insert(root, arr[i]);
    printvalues(root);
    return 0;
}