#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define len 256
typedef struct node
{
    int data;
    struct node *next;
}Node, *LinkedList;

LinkedList get_pos(char *, char);//get ch's position in the buffer string
void trail_insert(LinkedList, int);
void head_insert (LinkedList, int);

int main (int argc, char ** argv)
{
    LinkedList l = get_pos ("wo shi shui a", ' ');
    l = l -> next;
    while (l != NULL)
    {
        printf("%d\t", l ->data);
        l = l ->next;
    }
    //int *a = get_pos("wo shi shui a ", ' ');
}
void trail_insert(LinkedList l, int d)
{
    Node * a = (Node *)malloc (sizeof(Node));
    Node *p = l;
    while (p -> next != NULL)
        p = p ->next;
    a -> data = d;
    a ->next = p ->next;
    p -> next = a;
}

void head_insert (LinkedList l, int d)
{
    Node *a = (Node *)malloc (sizeof(Node));
    a -> data = d;
    a -> next = l -> next;
    l -> next = a;
}

LinkedList get_pos(char *buffer, char ch)//get ch's position in the buffer string
{
    LinkedList l = (Node *) malloc(sizeof(Node));
    l -> next =NULL; 
    int i;
    for (i = 0; i < strlen(buffer); i ++)
    {
        if(buffer[i] == ch)
            trail_insert(l, i);
    }
    return l;
}

