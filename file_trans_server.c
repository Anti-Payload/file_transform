#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#define maxlength 1024

int main(int argc, char *argv[])
{
    //build socket
    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t clilen;
    bzero( &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(13123);
    //bind & listen
    int server_socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socketfd < 0 )   perror("Fail to create socket");
    bind (server_socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(server_socketfd, 5);
    //server keep in loop structure
    while (1)
    {
        clilen = sizeof(client_addr);
        newsockfd = accept (server_socketfd, (struct sockaddr *) &client_addr, &clilen);
        char buffer [maxlength];// 待传输的文件名
        void *  buff_bin[1024];//buffer for binaray transform
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
        while ((re = fread(buff_bin, sizeof(void *), maxlength, fp)) != 0)
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


        
