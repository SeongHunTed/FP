#include <stdio.h>
#include <stdlib.h>

typedef struct _NODE{
    int data; 
    struct _NODE *ptr;
}Node;


void append(Node *Head, Node *a, int b)
{
    if(Head == NULL)
    {
        Node *newNode = malloc(sizeof(Node));
        newNode->data = b;
        Head = &newNode;
        newNode->ptr = NULL;
    } else {
        Node *newNode = malloc(sizeof(Node));
        newNode->ptr = a->ptr;
        a->ptr = &newNode;
    }
}

int main()
{
    Node *Head, *Tail = NULL;
    
    Node *a = malloc(sizeof(Node));
    a->data = 3;
    a->ptr = NULL;

    Head = &a;
    Tail = NULL;


    
}

