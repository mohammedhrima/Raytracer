#include <iostream>
#include <math.h>

typedef struct Node
{
    // int value;
    int x;
    int y;
    struct Node *left;
    struct Node *right;
} Node;

Node *newNode(int x, int y)
{
    Node *tmp = (Node *)calloc(1, sizeof(Node));
    tmp->x = x;
    tmp->y = y;
    return tmp;
}

void printvalues(Node *root)
{
    if (root != NULL)
    {
        printvalues(root->left);
        std::cout << "(" << root->x << " , " << root->y << ") " << std::endl;
        printvalues(root->right);
    }
}

int dist(int x, int y)
{
    return (int)sqrt(x * x + y * y);
}

Node *insert(Node *node, int x, int y)
{
    if (node == NULL)
        return newNode(x, y);

    int value1 = dist(node->x, node->y);
    int value2 = dist(x, y);

    if (value1 > value2)
        node->left = insert(node->left, x, y);
    else if (value1 <= value2)
        node->right = insert(node->right, x, y);
    return node;
}

int main()
{
    int arr[][2] = {
        {-2, -1},
        {-4, 3},
        {2, -1},
        {-1, 1},
        {4, 1},
        {3, -3},
        {2, 3}
    };

    int n = sizeof(arr) / sizeof(arr[0]);
    Node *root = NULL;

    root = insert(root, arr[0][0], arr[0][1]);
    for (int i = 1; i < n; i++)
        root = insert(root, arr[i][0], arr[i][1]);
    printvalues(root);
    return 0;
}