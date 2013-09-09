/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Your Name Here
* PROJECT:	CS 3251 Project 1 - Professor Traynor
* DESCRIPTION:	Network Server Code
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <openssl/evp.h>  /* for OpenSSL EVP digest libraries/SHA256 */

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/

/* The main function */
int main(int argc, char *argv[])
{

    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    socklen_t clntLen;			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store name from client */
    unsigned char md_value[EVP_MAX_MD_SIZE];	/* Buff to store change result */
    EVP_MD_CTX *mdctx;				/* Digest data structure declaration */
    const EVP_MD *md;				/* Digest data structure declaration */
    int md_len;					/* Digest data structure size tracking */

    ssize_t numBytes;

    /* Create new TCP Socket for incoming requests*/
    if ((serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Could not create socket.");
        exit(1);
    }

    /* Construct local address structure*/
    memset(&changeServAddr, 0, sizeof(changeServAddr));
    changeServAddr.sin_family = AF_INET;
    changeServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    changeServAddr.sin_port = htons(1234);
    
    /* Bind to local address structure */
    if (bind(serverSock, (struct sockaddr *) &changeServAddr, sizeof(changeServAddr)) < 0) {
        printf("bind() failed.");
        exit(1);
    }

    /* Listen for incoming connections */
    if (listen(serverSock, 5) < 0) {
        printf("listen() failed.");
        exit(1);
    }

    /* Loop server forever*/
    while(1) {
        /* Accept incoming connection */
        clntLen = sizeof(changeClntAddr);
        clientSock = accept(serverSock, (struct sockaddr *) &changeClntAddr, &clntLen);
        if (clientSock < 0) {
            printf("accept() failed.");
            exit(1);
        }

        /* Extract Your Name from the packet, store in nameBuf */
        numBytes = recv(clientSock, nameBuf, BUFSIZE - 1, 0);
        if (numBytes < 0) {
            printf("recv() failed.");
            exit(1);
        } else if (numBytes == 0) {
            printf("recv() connection closed prematurely.");
            exit(1);
        }

        /* Run this and return the final value in md_value to client */
        /* Takes the client name and changes it */
        /* Students should NOT touch this code */
        OpenSSL_add_all_digests();
        md = EVP_get_digestbyname("SHA256");
        mdctx = EVP_MD_CTX_create();
        EVP_DigestInit_ex(mdctx, md, NULL);
        EVP_DigestUpdate(mdctx, nameBuf, strlen(nameBuf));
        EVP_DigestFinal_ex(mdctx, md_value, &md_len);
        EVP_MD_CTX_destroy(mdctx);

        /* Return md_value to client */
        numBytes = send(clientSock, md_value, md_len, 0);
        if (numBytes < 0) {
            printf("send() failed.");
            exit(1);
        }
    }
}
