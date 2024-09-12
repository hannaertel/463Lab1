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

//function to open TCP socket following HTTP protocol
void open_TCP(const char *hostName, const char *filePath) {
    int sockfd, numbytes;
	struct sockaddr_in their_addr; /* client's address information */
	struct hostent* he;
    char request[1024];
    char buffer[4096];

    /* get server's IP by invoking the DNS */
	if ((he = gethostbyname(hostName)) == NULL) {
		herror("gethostbyname");
		exit(1);
	}

    //create socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

    their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(HTTPport);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	bzero(&(their_addr.sin_zero), 8);

    //connect to server
    if (connect(sockfd, (struct sockaddr *) &their_addr,
    sizeof(struct sockaddr)) < 0) {
		perror("connect");
		exit(1);
	}

    //http GET request
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\n"
             "Host: %s:%d\r\n" 
             "\r\n", 
             filePath, hostName, 80);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("send");
        exit(1);
    }

    //print response (to check)
    while ((numbytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[numbytes] = '\0';  // Null-terminate the received data
        printf("%s", buffer);     // Print the HTTP response
    }

    if (numbytes < 0) {
        perror("recv");
        exit(1);
    }

    close(sockfd);  

}


int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: ./http_client [host] [port number] [filepath]\n");
        exit(1);
    }

    open_TCP(argv[1], argv[2]);

    return 0;
}

