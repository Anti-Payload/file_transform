#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#define maxlength 1024
#include <unistd.h>
#include <arpa/inet.h>//function inet_ntoa inet_aton inet_addr



int main(int argc, char *argv[])
{
    //build socket
    int sockfd, newsockfd, portno;
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
        printf("recv connect ip:%s port:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        char buffer [maxlength];// 待传输的文件名
        unsigned char buff_bin[1024];//buffer for binaray transform
        bzero (buffer, sizeof(buffer) / sizeof (char));
        int rc = read(newsockfd, buffer, 1024);
        printf("the filename is %s\n", buffer);
        FILE * fp;
        if ((fp = fopen(buffer,"rb")) == NULL)
        {
        printf("no file name as %s\n", buffer);
        exit(1);
        }
        int re;
        while ((re = fread(buff_bin, sizeof(unsigned char), maxlength, fp)) != 0)
        {
            if (send(newsockfd, buff_bin, re, 0) < 0)
                printf("fail to send %s\n", buffer);
            bzero(buff_bin, maxlength);
        }
        fclose(fp);
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
