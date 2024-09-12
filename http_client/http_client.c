/* The code is subject to Purdue University copyright policies.
 * Do not share, distribute, or post online.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>


#define HTTPport 80

void extractName(const char *filePath, char *fileName) {
    //look for last occurence of / to get to name
    /* 
    const char *slash = strrchr(filePath, "/");

    //check that slash isnt null and that it isnt empty behind the slash
    if (slash && *(slash + 1)) {
        strcpy(fileName, slash + 1);
    } */

   char pathCopy[256];
   char *token;

   strncpy(pathCopy, filePath, sizeof(pathCopy )- 1);
   pathCopy[sizeof(pathCopy) - 1] = '\0';

   token = strtok(pathCopy, "/");
   while (token != NULL) {
    strcpy(fileName, token);
    token = strtok(NULL, "/");
   }

}

//function to open TCP socket following HTTP protocol
void open_TCP(const char *hostName, const char *filePath) {
    int sockfd, numBytes;
	struct sockaddr_in their_addr; /* client's address information */
	struct hostent* he;
    char responseStatus[128];
    char request[1024];
    char buffer[4096];
    char fileName[256];

   // get file name from file path
    extractName(filePath, fileName);
    //printf("File name: %s\n", fileName);

    //get servers IP
	if ((he = gethostbyname(hostName)) == NULL) {
        printf("error with host name");
		herror("gethostbyname");
		exit(1);
	}

    //create socket
    
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error with socket");
		perror("socket");
		exit(1);
	} 

    
    their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(HTTPport);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	bzero(&(their_addr.sin_zero), 8);

    //connect to server
    
    if (connect(sockfd, (struct sockaddr *) &their_addr, sizeof(struct sockaddr)) < 0) {
        printf("Error with server");
		perror("connect");
		exit(1);
	}

    //http GET request
    
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\nHost: %s\r\nConnection:close\r\n\r\n", 
             filePath, hostName);

    
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending HTTP request");
        exit(1);
    }
     


    FILE *file = fopen(fileName, "w");
    if(file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

     while ((numBytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[numBytes] = '\0';  // Null-terminate the received data
        fwrite(buffer, 1, numBytes, file);     // Print the HTTP response
    } 
   
    if (numBytes < 0) {
        perror("recv");
        exit(1);
    }

    fclose(file);


   /*
    if((numBytes == recv(sockfd, buffer, sizeof(buffer) - 1, 0)) <= 0) {
        printf("numBytes: %d", numBytes);
        perror("recv");
        exit(1);
    } */
    /*
    printf("got buffeer");

    buffer[numBytes] = '\0';
    //get first line
    sscanf(buffer, "%127[^r\n]", responseStatus);

    printf("after sscanf");

    //make sure 200 code is there
    
    if(strstr(responseStatus, "200 OK") == NULL) {
        //print first line and exit
        printf("%s\n", responseStatus);
        close(sockfd);
        return;
    }
    printf("200 OK worked"); */

    /*get file name from file path
    extractName(filePath, fileName);

    printf("File name: %s\n", fileName);

    */


    close(sockfd);  

}


int main(int argc, char *argv[])
{

    if (argc != 4) {
        fprintf(stderr, "usage: ./http_client [host] [port number] [filepath]\n");
        exit(1);
    }

    open_TCP(argv[1], argv[3]);

    return 0;
}

