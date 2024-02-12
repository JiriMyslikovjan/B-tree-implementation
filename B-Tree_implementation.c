#include <stdio.h>
#include <stdlib.h>

#define T 2
#define True 1
#define False 0
#define NO_KEY 0

// Structure for a node
// Includes an array of keys, an array of pointers to its children, if the node is a leaf and number of keys(n)

typedef struct NODE 
{
    int keys[(2 * T) - 1];
    struct NODE * children[2 * T];
    int leaf, n;
} node;

typedef struct BTREE
{
    node * root;
} bTree;

// Structure for returning search results
// Includes a pointer to a node and an index for a searched key 

typedef struct SEARCHRESULT
{
    node * node;
    int keyIndex;
} searchResult;

// Funtion for creating a new node
// Allocates memory accordingly, intializases arrays and vairables

node * createNewNode()
{
    node * newNode = NULL;
    newNode = (node *)malloc(sizeof(node));
    
    for(int i = 0; i < (2 * T) - 1; i++)
        newNode->keys[i] = NO_KEY; 
    
    for (int i = 0; i < 2 * T; i++)
        newNode->children[i] = NULL;
    
    newNode->leaf = True;
    newNode->n = 0;

    return newNode;
}

// Funtion for creating a B-tree
// Allocates memory and initializes root of a tree

bTree * createBTree()
{
    bTree * Tree = NULL;
    Tree = (bTree *)malloc(sizeof(bTree));
    node * x = createNewNode();
    Tree->root = x;

    return Tree; 
}

// Funcion splits children if needed for insertion

void splitChild(node * parent, int i)
{
    node * newNode1 = NULL;
    node * newNode2 = NULL;
    newNode1 = createNewNode();
    newNode2 = parent->children[i];
    newNode1->leaf = newNode2->leaf;
    newNode1->n = T - 1;
    newNode2->n = T - 1;

    for(int j = 0; j < T - 1; j++)
        newNode1->keys[j] = newNode2->keys[j + T];
    
    if(newNode2->leaf == False)
    {
        for(int j = 0; j < T; j++)
            newNode1->children[j] = newNode2->children[j + T];
    }

    for(int j = parent->n - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    for(int j = parent->n; j>= i + 1; j--)
        parent->children[j + 1] = parent->children[j];

    parent->children[i + 1] = newNode1;
    parent->keys[i] = newNode2->keys[T - 1];
    parent->n++;
}

// Function inserts a key into a non-full node

void treeInsertNonFull(node * currNode, int key)
{
    int i = currNode->n - 1;
    if(currNode->leaf)
    {
        while(i >= 0 && key < currNode->keys[i])
        {
            currNode->keys[i + 1] = currNode->keys[i];
            
            i--;
        }

        currNode->keys[i + 1] = key;
        currNode->n++;
    }

    else
    {
        while(i >= 0 && key < currNode->keys[i])
            i--;
        
        i++;

        if(currNode->children[i]->n == 2 * T - 1)
        {
            splitChild(currNode, i);
            
            if(key > currNode->keys[i])
                i++;       
        }
        
        treeInsertNonFull(currNode->children[i], key);
    }
}

// Funtion inserts a key into tree and splits a node if it's is full

void treeInsert(bTree * tree, int key)
{
    node * r = tree->root;
    if(r->n == (2 * T) - 1)
    {
        node * newNode = createNewNode();
        tree->root = newNode;
        newNode->leaf = False;
        newNode->n = 0;
        newNode->children[0] = r;

        splitChild(newNode, 0);

        treeInsertNonFull(newNode, key);
    }

    else
        treeInsertNonFull(tree->root, key);
}

// Function returns a structure including a node and an index at which the searched key was found 
// If key does not exist the function returns NULL and prints a statement that the key was not found

searchResult * bTreeSearch(node * currNode, int val)
{
    int i = 0;

    while(i < currNode->n && val > currNode->keys[i])
            i++;
    
    if(i < currNode->n && val == currNode->keys[i])
    {
        searchResult * result = (searchResult *)malloc(sizeof(searchResult));
        result->keyIndex = 0;
        result->node = NULL;

        printf("%d was found at index %d\n", currNode->keys[i], i);
        
        result->node = currNode;
        result->keyIndex = i;
        
        return result;
    }
    
    else if(currNode->leaf)
    {
        printf("Key %d was not found\n", val);

        return NULL;
    }
    
    else
        bTreeSearch(currNode->children[i], val);
}

// Returns an integer that has the next lowest value in comparison with a key at index in currNode


int getInorderPredecessor(node * currNode, int index)
{
    node * tmpNode = currNode->children[index];

    while(tmpNode->leaf == False)
        tmpNode = tmpNode->children[tmpNode->n];
    
    return tmpNode->keys[tmpNode->n - 1];
}

// Function returns an integer that has the next highest value in comparison with a key at index in currNode

int getInorderSuccessor(node * currNode, int index)
{
    node * tmpNode = currNode->children[index + 1];

    while(tmpNode->leaf == False)
        tmpNode = tmpNode->children[0];
    
    return tmpNode->keys[0];
}

// Funtion shifts a key from child1 to child2

void shiftLeft(node * parent, int index, node * child1, node * child2)
{
    child2->n++;
    child2->keys[child2->n - 1] = parent->keys[index];
    parent->keys[index] = child1->keys[0];
    
    for(int i = 0; i < child1->n - 1; i++)
        child1->keys[i] = child1->keys[i + 1];

    if(child1->leaf == False)
    {
        child2->children[child2->n] = child1->children[0];
        for(int i = 0; i < child1->n; i++)
            child1->children[i] = child1->children[i + 1];
    }

    child1->n--;
}

// Funtion shifts a key at from child1 to child2

void shiftRight(node * parent, int index, node * child1, node * child2)
{
    child2->n++;
    
    for(int i = child2->n - 1; i > 0; i--)
        child2->keys[i] = child2->keys[i - 1];
    
    child2->keys[0] = parent->keys[index];
    parent->keys[index] = child1->keys[child1->n - 1];

    if(child1->leaf == False)
    {
        for (int i = child2->n; i > 0; i--)
            child2->children[i] = child2->children[i - 1];
    }
    
    child2->children[0] = child1->children[child1->n];
    child1->n--;
}

// Function merges parent node with it's two children nodes 

void merge(node * parent, int index, node * child1, node * child2)
{
    child1->n = (T * 2) -1;
    
    for (int i = T; i < (T * 2) - 1; i++)
        child1->keys[i] = child2->keys[i - T];
    
    child1->keys[T - 1] = parent->keys[index];

    if(child2->leaf == False)
    {
        for (int i = T; i < T * 2; i++)
            child1->children[i] = child2->children[i - T];  
    }

    for(int i = index + 1; i < parent->n; i++)
    {
        parent->keys[i - 1] = parent->keys[i];
        parent->children[i] = parent->children[i + 1];
    }
    
    parent->n--;

    free(child2);
}

// Function deletes a key from a node that is a leaf

void deleteFromLeaf(int keyToDel, node * currNode)
{
    int index = 0;

    while(currNode->keys[index] < keyToDel)
        index++;

    for(int i = index;i < currNode->n - 1; i++)
        currNode->keys[i] = currNode->keys[i + 1];
    
    currNode->n--;   
}

// Funtion deletes a key from a node calls deleteFromLeaf if key was found in a leaf.
// If key is in an internal node then it deletes said key and fixes nodes by merging if needed 

void deleteFromNode(node * currNode, int keyToDel)
{
    if(currNode->leaf)
        deleteFromLeaf(keyToDel, currNode);
    
    else
    {
        int i = 0;
        while(currNode->n > i && keyToDel > currNode->keys[i])
                i++;

        node * leftChild = currNode->children[i];
        node * rightChild = currNode->children[i + 1];

        if(keyToDel == currNode->keys[i])
        {   
            if(leftChild->n > T - 1)
            {
                int predecessor = getInorderPredecessor(currNode, i);
                currNode->keys[i] = predecessor;

                deleteFromNode(leftChild, predecessor);
            }
            
            else if(rightChild->n > T - 1)
            {
                int successor = getInorderSuccessor(currNode, i);
                currNode->keys[i] = successor;

                deleteFromNode(rightChild, successor);
            }

            else
            {
                merge(currNode, i, leftChild, rightChild);
                deleteFromNode(leftChild, keyToDel);
            }
        }

        else
        {
            node * leftChildSibling = NULL; 
            rightChild = NULL;
            if(i < currNode->n)
                rightChild = currNode->children[i + 1];
            
            if(i > 0)
                leftChildSibling = currNode->children[i - 1];
            
            if(leftChild != NULL && T - 1 == leftChild->n)
            {
                if(leftChildSibling != NULL && leftChildSibling->n > T - 1)
                    shiftRight(currNode, i - 1, leftChildSibling, leftChild);

                else if(rightChild != NULL && rightChild->n > T - 1)
                    shiftLeft(currNode, i, rightChild, leftChild);

                else if(leftChildSibling != NULL)
                {
                    merge(currNode, i - 1, leftChildSibling, leftChild);
                    leftChild = leftChildSibling;
                }

                else
                    merge(currNode, i, leftChild, rightChild);
            }
            deleteFromNode(leftChild, keyToDel);
        }
    }
}


// Funtion checks if desired key exists in tree. If it's present deletes it, if not it prints out a statement

node *  bTreeDelete(bTree * tree, int keyToDel)
{
    if(bTreeSearch(tree->root, keyToDel) != NULL)
    {
        if(tree->root->n == 1 
        && tree->root->children[0] != NULL
        && tree->root->children[1] != NULL
        && tree->root->children[0]->n == T - 1
        && tree->root->children[1]->n == T - 1)
        {
            node * leftChild = tree->root->children[0];
            node * rightChild = tree->root->children[1];

            merge(tree->root, 0, leftChild, rightChild);
            free(tree->root);
            deleteFromNode(leftChild, keyToDel);
            return leftChild;
        }

        else
        {
            deleteFromNode(tree->root, keyToDel);
            return tree->root;
        }
    }

    else
    {
        printf("Key for deletion was not found\n");
        return tree->root;
    }
    
}

int main()
{
    bTree * myTree = createBTree();
    
    // Insertion

    treeInsert(myTree, 1);
    treeInsert(myTree, 2);
    treeInsert(myTree, 3);
    treeInsert(myTree, 4);
    treeInsert(myTree, 5);
    treeInsert(myTree, 6);
    treeInsert(myTree, 7);
    treeInsert(myTree, 8);
    treeInsert(myTree, 9);
    treeInsert(myTree, 10);
    treeInsert(myTree, 11);
    treeInsert(myTree, 12);
    treeInsert(myTree, 13);
    treeInsert(myTree, 14);
    treeInsert(myTree, 15);
    treeInsert(myTree, 16);
    treeInsert(myTree, 17);
    treeInsert(myTree, 18);
    treeInsert(myTree, 19);
    treeInsert(myTree, 20);
    treeInsert(myTree, 21);
    treeInsert(myTree, 22);

    // deletion    

    myTree->root = bTreeDelete(myTree, 4);
    myTree->root = bTreeDelete(myTree, 22);
    myTree->root = bTreeDelete(myTree, 8);
    myTree->root = bTreeDelete(myTree, 16);
    myTree->root = bTreeDelete(myTree, 17);
    myTree->root = bTreeDelete(myTree, 14);
    myTree->root = bTreeDelete(myTree, 1);
    myTree->root = bTreeDelete(myTree, 2);
    myTree->root = bTreeDelete(myTree, 15);
    myTree->root = bTreeDelete(myTree, 10);
    myTree->root = bTreeDelete(myTree, 11);
    myTree->root = bTreeDelete(myTree, 9);

    printf("\nafter deletion:\n");

    bTreeSearch(myTree->root, 1);
    bTreeSearch(myTree->root, 2);
    bTreeSearch(myTree->root, 3);
    bTreeSearch(myTree->root, 4);
    bTreeSearch(myTree->root, 5);
    bTreeSearch(myTree->root, 6);
    bTreeSearch(myTree->root, 7);
    bTreeSearch(myTree->root, 8);
    bTreeSearch(myTree->root, 9);
    bTreeSearch(myTree->root, 10);
    bTreeSearch(myTree->root, 11);
    bTreeSearch(myTree->root, 12);
    bTreeSearch(myTree->root, 13);
    bTreeSearch(myTree->root, 14);
    bTreeSearch(myTree->root, 15);
    bTreeSearch(myTree->root, 16);
    bTreeSearch(myTree->root, 17);
    bTreeSearch(myTree->root, 18);
    bTreeSearch(myTree->root, 19);
    bTreeSearch(myTree->root, 20);
    bTreeSearch(myTree->root, 21);
    bTreeSearch(myTree->root, 22);

    return 0;
}