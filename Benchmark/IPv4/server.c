#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define MAXSIZE 1024


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,ret = 0;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in client_addr; 
    int cliaddrlen = 0;
        struct timeval tv;
    unsigned long time_in_micros;

    char sendBuff[MAXSIZE];
    char recvBuff[MAXSIZE];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&client_addr, '0', sizeof(client_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.17.2");
    serv_addr.sin_port = htons(5000); 

    //printf("The val of IP addr is %s\n", inet_ntoa(serv_addr.sin_addr.s_addr));
    cliaddrlen = sizeof(client_addr);
    ret = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(ret <0 )
        perror("bind failed");

    {
        printf("Receiving...\n");
        recvfrom(listenfd,recvBuff,21,0,(struct sockaddr *)&client_addr,&cliaddrlen);
   gettimeofday(&tv,NULL);

   time_in_micros = (1000000 * tv.tv_sec) + tv.tv_usec;
   printf("The time is %lu\n",time_in_micros);
        printf("Received IP is %s\n", inet_ntoa(client_addr.sin_addr));

        close(listenfd);
    }
}
