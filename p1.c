/*Nick Chao, 111806065, CMSC417
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
 
int main(int argc, char *argv[]){
    char *inputBuff;
    size_t charsRead = 0;
    int curr_byte;
    size_t buffSize = 255;
 
    if(argc != 3){
        fprintf(stderr, "Incorrect number of arguments!\n");
        exit(1);
    }
 
    char *servIP = argv[1];
    char *port = argv[2];
    in_port_t servPort = atoi(argv[2]);
 
    //Create socket
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0){
        fprintf(stderr, "Creating a socket failed\n");
        exit(1);
    }
 
    //sets up addrinfo struct to use with getaddrinfo()
    /*Code from TCP/IP Sockets in C by Michael Donahoo and Kenneth Calvert*/
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;
 
    /*Code from TCP/IP Sockets in C by Michael Donahoo and Kenneth Calvert*/
    struct addrinfo *addrList;
    int rtnVal = getaddrinfo(servIP, port, &addrCriteria, &addrList);
    if(rtnVal != 0){
        fprintf(stderr, "The getaddrinfo() function failed\n");
        exit(1);
    }
     
    /*Constructs server address structure*/
    /*Code from TCP/IP Sockets in C by Michael Donahoo and Kenneth Calvert*/
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
 
    struct sockaddr_in *resolvedAddr = (struct sockaddr_in*)addrList->ai_addr;
 
    //inet_ntoa converts IPv4 address from the resolved addr struct into ASCII string
    //inet_pton converts string of address into a 32-bit binary representation
    int rtn = inet_pton(AF_INET, inet_ntoa(resolvedAddr->sin_addr), &servAddr.sin_addr.s_addr);
    if(rtn == 0){
        fprintf(stderr, "Invalid address string\n");
        exit(1);
    }else if (rtn < 0){
        fprintf(stderr, "inet_pton failed\n");
        exit(1);
    }

    /*Code from TCP/IP Sockets in C by Michael Donahoo and Kenneth Calvert*/
    //sets the Port number in the serverAddr struct
    servAddr.sin_port = htons(servPort);
    //Attempt to connect to server
    if(connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
        fprintf(stderr, "Connect failed\n");
        exit(1);
    }
     
    //attempts to allocate memory for the input buffer
    inputBuff = malloc(sizeof(char)*buffSize);
    if(inputBuff == NULL){
        fprintf(stderr, "Malloc failed for inputBuff\n");
        exit(1);
    }
     
    //read stdin char by char until it reaches EOF
    while( (curr_byte = fgetc(stdin)) != EOF){
        inputBuff[charsRead++] = curr_byte;
 
        if(charsRead == buffSize){
            //doubles the sie of the buffer if necessary
            buffSize+=(buffSize*2); 
            inputBuff = realloc(inputBuff, sizeof(char)*(buffSize));
            if(inputBuff == NULL){
                fprintf(stderr, "Reallocation of memory failed while reading from stdin\n");
                exit(1);
            }
        }
    }
    //Adds null byte at the end of the string and reallocates
    //the proper amount of memory for the string
    inputBuff[charsRead++] = '\0';
    if(realloc(inputBuff, sizeof(char)*(charsRead)) == NULL){
        fprintf(stderr, "realloc for final inputBuff didn't work\n");
        exit(1);
    }
 
    //Attempts to write the inputBuffer to the socket
    /*Code taken from the book "TCP/IP Sockets in C"
      Authors: Michael Donahoo and Kenneth Calvert
    */
    if(write(sock, inputBuff, charsRead) < 0){
        printf("Writing to the socket failed!");
        exit(1);
    }
 
    //Attempts to shutdown the writing ability for the socket
    if(shutdown(sock, SHUT_WR) < 0){
        fprintf(stderr, "Shutdown for writing to the socket failed\n");
        exit(1);
    }
 
    //Sets up buffer in order to read from the socket
    int readResult;
    char *readBuff = malloc(sizeof(char)*1024);
    if(readBuff == NULL){
        fprintf(stderr, "Malloc failed for the buffer for reading from the socket\n");
        exit(1);
    }
 
    //Read from the socket until it reaches the EOF
    while((readResult = read(sock, readBuff, sizeof(readBuff))) != 0){
        if(readResult < 0){
            fprintf(stderr, "read fail");
            exit(1);
        }
        printf("%s", readBuff);

        //reset the buffer
        if(realloc(readBuff, sizeof(readBuff)) == NULL) {
            fprintf(stderr, "Realloc failed inside while loop for reading from the socket");
            exit(1);
        }
        memset(readBuff, 0, sizeof(readBuff));
    }
    close(sock);
    exit(0);
}
