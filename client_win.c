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
    client_addr.sin_port = htons(0);//�ƶ�����˿���Ϊ�ͻ��˵Ķ˿�

    // ����socket�����ɹ�������socket������
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    // �󶨿ͻ��˵�socket�Ϳͻ��˵�socket��ַ�ṹ �Ǳ���
    /*if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }*/

    // ����һ���������˵�socket��ַ�ṹ�����÷������Ǳߵ�IP��ַ���˿ڶ�����г�ʼ�������ں��������
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);//line 56 ����һ��ipָ������
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
    // ��������������ӣ����ӳɹ���client_socket_fd�����˿ͻ��˺ͷ�������һ��socket����
    if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Can Not Connect To Server IP:");
        exit(0);
    }

    // �����ļ��� ���ŵ�������buffer�еȴ�����
    char file_name[FILE_NAME_MAX_SIZE+1];
    bzero(file_name, FILE_NAME_MAX_SIZE+1);
    printf("Please Input File Name On Server:\t");
    scanf("%s", file_name);

    char buffer[BUFFER_SIZE];
    unsigned char buff_bin[BUFFER_SIZE];//buffer for binaray transform
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));

    // �����������buffer�е�����
    if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
    {
        perror("Send File Name Failed:");
        exit(1);
    }

    // ���ļ���׼��д��
    FILE *fp = fopen(file_name, "wb");
    if(NULL == fp)
    {
        printf("File:\t%s Can Not Open To Write\n", file_name);
        exit(1);
    }

    // �ӷ������������ݵ�buffer��
    // ÿ����һ�����ݣ��㽫��д���ļ��У�ѭ��ֱ���ļ������겢д��Ϊֹ
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

    // ���ճɹ��󣬹ر��ļ����ر�socket
    printf("Receive File:\t%s From Server IP Successful!\n", file_name);
    fclose(fp);
    close(client_socket_fd);
    return 0;
}

int *get_space(char *buffer)//��ȡ�ַ����ո�λ�ã�����ֵ�Ǵ���λ�õ�����
{
    int i;
    int blank_num = 1;//blank_pos[0] = 0,���������������㡣
    static int blank_pos [Len];//��̬������һֱ�����ͷ�
    memset(blank_pos, -1, sizeof(blank_pos)); //����-1 ������ڼ����ַ�������
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

char * split_str(char *buffer)//ģ��split�������ַ�����ֳ��ַ������飬�Կո���Ч��
{
    int *a = get_space(buffer);
    int len = get_len(a);
    int last_tail = strlen(buffer);
    char *words[len + 1];// ��ά���飬�洢�ִ�
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
