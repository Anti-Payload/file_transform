#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winsock.h>
#include <ctype.h>
#include <winsock2.h>
#include <windows.h>
#define Len 256
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define bzero(a, b) memset(a, 0, b)
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int *get_space(char *);
int get_len(int *);
char *split_str(char *);

int main(int argc, char *argv[])
{
    //char ** words = split_str("wo shishi you jige kongge hahahaha");
    WSADATA wsa;
    SOCKET client_socket_fd ;
    printf("\nInitialising Winsock...");

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    struct sockaddr_in client_addr;
    int portno;
    portno = atoi(argv[2]);
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);//制定任意端口作为客户端的端口

    // 创建socket，若成功，返回socket描述符
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    // 绑定客户端的socket和客户端的socket地址结构 非必需
    /*if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }*/

    // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);//line 56 是另一种ip指定方法
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    /*if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0)
    {
        perror("Server IP Address Error:");
        exit(1);
    }
    /*struct hostent *server;
    server = gethostbyname(argv[1]);
    bcopy((char *)server -> h_addr, (char *)&server_addr.sin_addr.s_addr, server -> h_length);*/
    int server_addr_length = sizeof(server_addr);
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
    scanf("%s", file_name);

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

int *get_space(char *buffer)//获取字符串空格位置，返回值是储存位置的数组
{
    int i;
    int blank_num = 1;//blank_pos[0] = 0,方便其他函数计算。
    static int blank_pos [Len];//静态变量，一直不会释放
    memset(blank_pos, -1, sizeof(blank_pos)); //设置-1 方便后期剪切字符串方便
    for (i = 0; i < strlen(buffer); i ++)
    {
        if (isblank(buffer[i]))
        {
            printf("the %dth num is %d\n", blank_num, i);
            blank_pos[blank_num ++] = i;
        }
    }
    return blank_pos;
}

char * split_str(char *buffer)//模拟split操作，字符串拆分成字符串数组，对空格有效。
{
    int *a = get_space(buffer);
    int len = get_len(a);
    int last_tail = strlen(buffer);
    char *words[len + 1];// 二维数组，存储分词
    int i;
    for (i = 0; i < len  ; i ++)
    {
        if (i < len -1)
        {
            int str_length = a[i+1] - a[i] -1 ;
            words[i] = (char *)malloc(str_length);
            strncpy(words[i], buffer,str_length);
            words[i][str_length] = '\0';
            buffer += (str_length + 1);
            //printf("%s's length is %d\n", words[i], strlen(words[i]));
        }
        else
        {
            int str_length = last_tail - a[len-1] -1;
            words[i] = (char *)malloc(str_length);
            strncpy(words[i], buffer,str_length);
            words[i][str_length] = '\0';
            buffer += (str_length + 1);
            //printf("%s's length is %d\n", words[i], strlen(words[i]));
        }
    }
    return words;
}

int get_len(int *a)
{
    int len = 1, i;
    for (i = 1; a[i] != -1; i ++)
        len ++;
    printf("there're %d spaces in the option-code\n",len);
    return len;
}
