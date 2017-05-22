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
char *split_str(char *, char);

int main (int argc, char ** argv)
{
    char ** a = split_str("wo shi shui a haha", ' ');
    printf("%s\n%s\n%s\n%s\n%s\n", a[0], a[1], a[2], a[3], a[4]);
    return 0;
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
    l -> data ++;// record number of nodes
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
    l -> data = 0;
    l -> next =NULL; 
    int i;
    for (i = 0; i < strlen(buffer); i ++)
    {
        if(buffer[i] == ch)
            trail_insert(l, i);
    }
    printf("there're %d nodes in the linkedlist\n",l -> data);
    return l;
}

char *split_str(char *buffer, char ch)
{
    LinkedList l = get_pos (buffer, ch);
    char *words[l -> data +1];
    int i;
    int words_num = l -> data;
    int pre_pos = -1;
    l = l -> next;
    for (i = 0; i <= words_num; i ++)
    {
        int str_length;
        if (i != words_num)
        {
            str_length = l -> data - pre_pos -1;
            words[i] = (char *)malloc (str_length);
            strncpy (words[i], (buffer + pre_pos + 1), str_length);
            words[i][str_length] = '\0';
            //printf("pos is %d and  string is %s\n", pre_pos, words[i]);
            pre_pos =l -> data;

        }
        else
        {
            //printf("last one ");
            str_length = strlen(buffer) - pre_pos ;
            words[i] = (char *)malloc (str_length + 1);
            strcpy(words[i], (buffer +pre_pos + 1));
            //printf("%s\n",words[i]);
            //printf("pos is %d and  string is %s\n", pre_pos, words[i]);
        }
        if (l != NULL)  l = l->next;
    }

    return words;
}
