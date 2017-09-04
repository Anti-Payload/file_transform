#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#define maxlength 1024
#include <unistd.h>
#include <arpa/inet.h>//function inet_ntoa inet_aton inet_addr
#define Len 256
#define ERR_EXIT(m)\
do {\
    perror(m);\
    exit(-1);\
}while(0)\

const char *opcodes[] = 
{
    "getfile",
    "putfile",
    "sendmsg",
};//op-code list

typedef struct node
{
    int data;
    struct node *next;
}Node, *LinkedList;


LinkedList get_pos(char *, char);//get ch's position in the buffer string
int match_opcode(char *, const char **);//match opcode in codelist
void trail_insert(LinkedList, int);// 尾插链表用来存放字符在字符串中的位置
void head_insert (LinkedList, int);
char **split_str(char *,const char);//函数返回分解的字符串数组

int main(int argc, char *argv[])
{
    //build socket
    int  newsockfd, portno;
    pid_t p;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t clilen;
    portno = atoi(argv[1]);
    bzero( &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(portno);
    //bind & listen
    int server_socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socketfd < 0 )   perror("Fail to create socket");
    bind (server_socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(server_socketfd, 5);
    //server keep in loop structure
    while (1)
    {
        clilen = sizeof(client_addr);
        if ((newsockfd = accept (server_socketfd, (struct sockaddr *) &client_addr, &clilen) ) < 0)
            perror ("accept error:");
        pid_t pid = fork();
        if (pid == -1)  ERR_EXIT("fork error ");
        else if (pid > 0)
        {
            close(newsockfd);
        }
        else
        {
            close(server_socketfd);
            printf("recv connect ip:%s port:%d by process %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), getpid());
            char buffer [maxlength];//buffer for opcode recieve from client 
            int rc = read(newsockfd, buffer, 1024);
            char **codes = split_str(buffer, ' ');
            printf("%s and %s\n",codes[0], codes[1]);
            char *s = codes[0];//这里我不太明白为什么codes 的地址会变化。如果输出codes[0]和codes[1],两次的结果就会不一样。
            char *fname = codes[1];
            int flag = match_opcode(s, opcodes);
            printf("%s and %s\n", s, fname);
            printf("%s and %s\n", codes[0], codes[1]);
            switch (flag)
            {
                case 0://getfile
                    printf("the filename is %s\n", fname);
                    unsigned char buff_bin[1024];//buffer for binaray transform
                    bzero (buffer, sizeof(buffer) / sizeof (char));
                    FILE * fp;
                    if ((fp = fopen(fname,"rb")) == NULL)
                    {
                        printf("no file name as %s\n", codes[1]);
                        exit(-1);
                    }
                    int re;
                    while ((re = fread(buff_bin, sizeof(unsigned char), maxlength, fp)) != 0)
                    {
                        if (send(newsockfd, buff_bin, re, 0) < 0)
                            printf("fail to send %s\n", fname);
                        bzero(buff_bin, maxlength);
                    }
                    fclose(fp);
                    break;
                case 1://putfile
                    break;
                case 2://sendmag
                    break;
                default:
                    printf("wrong opcode of %s\n", codes[1]);
                    break;
            }
            printf("done\n");
            return 0;//multi process should exit child-process
        }
    }
    return 0;
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

char** split_str(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int match_opcode(char *opcode,const char **codelist)
{
    int i = 0, flag = -1;
    while (strlen(codelist[i]) != 0 && codelist[i][0] != '\0')
    {
        if (strcmp(opcode, codelist[i]) == 0)
        {
            flag = i;
            break;
        }
        i ++;
    }
    return flag;
}


