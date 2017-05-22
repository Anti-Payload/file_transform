/*************************************************************************
    > File Name: file_trans_client.c
    > Author: gary_chang
    > E-mail: whitehat0817@gmail.com
    > Created Time: 2017年05月10日  13时07分10秒
 ************************************************************************/
#include<netinet/in.h>   // sockaddr_in
#include<sys/types.h>    // socket
#include<sys/socket.h>   // socket
#include<stdio.h>        // printf
#include<stdlib.h>       // exit
#include<string.h>       // bzero
#include <unistd.h>
#include <netdb.h>


#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
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
void trail_insert(LinkedList, int);// 尾插链表用来存放字符在字符串中的位置
void head_insert (LinkedList, int);
char *split_str(char *, char);//函数返回分解的字符串数组


int main(int argc, char *argv[])
{
    // 声明并初始化一个客户端的socket地址结构
    struct sockaddr_in client_addr;
    int portno;
    portno = atoi(argv[2]);
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);//制定任意端口作为客户端的端口

    // 创建socket，若成功，返回socket描述符
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    // 绑定客户端的socket和客户端的socket地址结构 非必需
    if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }

    // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);//line 56 是另一种ip指定方法
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0)
    {
        perror("Server IP Address Error:");
        exit(1);
    }
    /*struct hostent *server;
    server = gethostbyname(argv[1]);
    bcopy((char *)server -> h_addr, (char *)&server_addr.sin_addr.s_addr, server -> h_length);*/
    socklen_t server_addr_length = sizeof(server_addr);
    printf("start to connect %s at port %s\n",argv[1], argv[2]);
    // 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
    if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Can Not Connect To Server IP:");
        exit(0);
    }

    // 输入文件名 并放到缓冲区buffer中等待发送
    char file_name[FILE_NAME_MAX_SIZE+1];
    bzero(file_name, FILE_NAME_MAX_SIZE+1);
    printf("Please Input File Name On Server:\t");
    gets(file_name);
    printf("%s", file_name);

    char buffer[BUFFER_SIZE];
    unsigned char buff_bin[BUFFER_SIZE];//buffer for binaray transform
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));
    
    // 向服务器发送buffer中的数据
    if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
    {
        perror("Send File Name Failed:");
        exit(1);
    }

    // 打开文件，准备写入
    FILE *fp = fopen(file_name, "wb");
    if(NULL == fp)
    {
        printf("File:\t%s Can Not Open To Write\n", file_name);
        exit(1);
    }

    // 从服务器接收数据到buffer中
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
    bzero(buffer, BUFFER_SIZE);
    int length = 0;
    while((length = recv(client_socket_fd, buff_bin, BUFFER_SIZE, 0)) > 0)
    {
        if(fwrite(buff_bin, sizeof(unsigned char), length, fp) < length)
        {
            printf("File:\t%s Write Failed\n", file_name);
            break;
        }
        bzero(buff_bin, BUFFER_SIZE);
    }

    // 接收成功后，关闭文件，关闭socket
    printf("Receive File:\t%s From Server IP Successful!\n", file_name);
    fclose(fp);
    close(client_socket_fd);
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

