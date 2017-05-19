#include <stdio.h>
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

int *get_space(char *);//获取字符串空格位置，返回值是储存位置的数组
char * split_str(char *);//模拟split操作，字符串拆分成字符串数组，对空格有效。
int get_len(int *);

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
            char buffer [maxlength];// 待传输的文件名
            unsigned char buff_bin[1024];//buffer for binaray transform
            bzero (buffer, sizeof(buffer) / sizeof (char));
            int rc = read(newsockfd, buffer, 1024);
            printf("the filename is %s\n", buffer);
            FILE * fp;
            if ((fp = fopen(buffer,"rb")) == NULL)
            {
                printf("no file name as %s\n", buffer);
                exit(-1);
            }
            int re;
            while ((re = fread(buff_bin, sizeof(unsigned char), maxlength, fp)) != 0)
            {
                if (send(newsockfd, buff_bin, re, 0) < 0)
                    printf("fail to send %s\n", buffer);
                bzero(buff_bin, maxlength);
            }
            fclose(fp);
            return 0;//multi process should exit child-process
        }
    }
    return 0;
}


int *get_space(char *buffer)//获取字符串空格位置，返回值是储存位置的数组
{
    int i;
    int blank_num = 1;//blank_pos[0] = 0,方便其他函数计算。
    static int blank_pos[Len];//静态变量，一直不会释放
    memset(blank_pos, -1, sizeof(blank_pos)); //设置-1 方便后期剪切字符串方便
    for (i = 0; i < strlen(buffer); i++)
    {
        if (isblank(buffer[i]))
        {
            printf("the %dth num is %d\n", blank_num, i);
            blank_pos[blank_num++] = i;
        }
    }
    return blank_pos;
}

char * split_str(char *buffer)//模拟split操作，字符串拆分成字符串数组，对空格有效。
{
    int *a = get_space(buffer);
    int len = get_len(a);
    len ++;
    int last_tail = strlen(buffer);
    char *words[len];// 二维数组，存储分词
    int i;
    for (i = 0; i < len; i++)
    {
        if (i < len - 1)
        {
            int str_length = a[i + 1] - a[i] - 1;
            words[i] = (char *)malloc(str_length);
            strncpy(words[i], buffer, str_length);
            words[i][str_length] = '\0';
            buffer += (str_length + 1);
            //printf("%s's length is %d\n", words[i], strlen(words[i]));
        }
        else
        {
            int str_length = last_tail - a[len - 1] - 1;
            words[i] = (char *)malloc(str_length);
            strncpy(words[i], buffer, str_length);
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
    for (i = 1; a[i] != -1; i++)
        len++;
    printf("there're %d spaces in the option-code\n", len);
    return len;
}

