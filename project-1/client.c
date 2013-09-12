/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Patrick Stoica
* PROJECT:	CS 3251 Project 1 - Professor Traynor
* DESCRIPTION:	Network Client Code
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>	    /* for OpenSSL EVP digest libraries/SHA256 */

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define MDLEN 32

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */

    char *studentName;		    /* Your Name */

    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    int i;			    /* Counter Value */

    ssize_t numBytes;

    /* Get the Student Name from the command line */
    if (argc != 2) {
        printf("Incorrect input format. The correct format is:\n\tnameChanger your_name\n");
        exit(1);
    }

    studentName = argv[1];
    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);
    strcpy(sndBuf, studentName);

    /* Create a new TCP socket*/
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock < 0) {
        printf("Could not create socket.");
        exit(1);
    }

    /* Construct the server address structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1234);

    /* Establish connection to the server */
    if (connect(clientSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Could not connect to server.");
        exit(1);
    }

    /* Send the string to the server */
    numBytes = send(clientSock, sndBuf, SNDBUFSIZE, 0);
    if (numBytes < 0) {
        printf("send() failed.");
        exit(1);
    }

    /* Receive and print response from the server */
    unsigned int totalBytesRcvd = 0;

    printf("%s\n", studentName);
    printf("Transformed input is: ");

    while (totalBytesRcvd < MDLEN) {
        memset(&rcvBuf, 0, RCVBUFSIZE);

        numBytes = recv(clientSock, rcvBuf, RCVBUFSIZE - 1, 0);
        if (numBytes < 0) {
            printf("recv() failed.");
            exit(1);
        } else if (numBytes == 0) {
            printf("recv() connection closed prematurely.");
            exit(1);
        }

        for(i = 0; i < numBytes; i++) printf("%02x", (unsigned char) rcvBuf[i]);

        totalBytesRcvd += numBytes;
    }

    printf("\n");

    return 0;
}
