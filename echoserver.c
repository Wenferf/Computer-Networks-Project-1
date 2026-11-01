/*
 * echoserver.c - A simple connection-based echo server
 * usage: echoserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define QSIZE 5

// default values for optional arguments.
#define DEFAULT_PORT 2012
#define DEFAULT_RATE_REQUESTS 3
#define DEFAULT_RATE_SECONDS 60
#define DEFAULT_MAX_USERS 3
#define DEFAULT_TIMEOUT_SECONDS 80

// max values to protect server
#define MAX_FILE_SZ 40 // in MBs
#define MAX_URL_SZ  200 // in characters

#if 0
/*
 * Structs exported from netinet/in.h (for easy reference)
 */

/* Internet address */
struct in_addr {
  unsigned int s_addr;
};

/* Internet style socket address */
struct sockaddr_in  {
  unsigned short int sin_family; /* Address family */
  unsigned short int sin_port;   /* Port number */
  struct in_addr sin_addr;	 /* IP address */
  unsigned char sin_zero[...];   /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
  char    *h_name;        /* official name of host */
  char    **h_aliases;    /* alias list */
  int     h_addrtype;     /* host address type */
  int     h_length;       /* length of address */
  char    **h_addr_list;  /* list of addresses */
}
#endif

// initialize all config parameters
int portno = DEFAULT_PORT;
// do same for all other args.

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

// todo:: Phase 4
void process_args(int argc, char **argv) {
  // initialize all argumesnts.
  // create DEFINES, set arg varaibles to those.
  // also do validation.
  // while loop to process all optional args.
}

void process_client_request(int connfd, struct sockaddr_in clientaddr) {
  u_int32_t fsize;

  char *hostaddrp; /* dotted decimal host addr string */
  struct hostent *hostp; /* client host info */
  int hostport; /* client port */


    char hostname[NI_MAXHOST];
    char portname[NI_MAXSERV];

    // Look up both hostname and port safely
    int s = getnameinfo((struct sockaddr *)&clientaddr, sizeof(struct sockaddr_in),
                        hostname, sizeof(hostname),
                        portname, sizeof(portname),
                        0); // Use NI_NAMEREQD flag if you want it to strictly fail without DNS

/*  TODO: It seems gethostbyaddr is deprecated. But new recommeneded method is showing different results...
    // gethostbyaddr: determine who sent the message 
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");

    hostport = ntohs(clientaddr.sin_port);
    printf("server established connection with %s (%s):%d\n",
	   hostp->h_name, hostaddrp, hostport);
     
     */
    printf("server established connection with %s:%s\n",
	   hostname, portname);

   /* read: file size from the client */

    // get file size.
    if (recv (connfd, &fsize, sizeof (u_int32_t), 0) < 0) {
      printf("ERROR reading fsize from socket");
          return;
    }
    fsize = ntohl(fsize);

    if (fsize > BUFSIZE - 1) {
    fprintf(stderr, "ERROR: oversized file: %u bytes\n", fsize);
    return;
}

   
    // get file content
    char buffer[BUFSIZE]; buffer[fsize] = '\0';
int  bytesActuallyReceived;
u_int32_t bytesReceivedsoFar = 0;

while (bytesReceivedsoFar < fsize) {
  bytesActuallyReceived = recv(connfd, buffer+bytesReceivedsoFar, fsize-bytesReceivedsoFar,0);

      // 1. Check for system/network errors
    if (bytesActuallyReceived < 0) {
        printf("Network error during recv");
        break;
    }
    
    // 2. Check if the client disconnected too early
    if (bytesActuallyReceived == 0) {
        fprintf(stderr, "Error: Client closed connection prematurely at %u / %u bytes.\n", 
                bytesReceivedsoFar, fsize);
        break;
    }
  
  bytesReceivedsoFar += bytesActuallyReceived;
}

printf("DEBUG AFTER - server received %u bytes from client %s:%s.\n", bytesReceivedsoFar, hostname, portname);
/*
if (bytesReceivedsoFar == fsize) {
    printf("SUCCESS: File completely received! Total bytes: %u / %u\n", bytesReceivedsoFar, fsize);
    
    // --- START OF FILE DUMP CODE ---
    
    // 1. Open the file in Binary Write mode ("wb")
    FILE *fp = fopen("output_debug.txt", "wb"); 
    if (fp == NULL) {
        perror("Error opening file for writing");
    } else {
        // 2. Write the EXACT number of bytes received from the buffer
        size_t bytesWritten = fwrite(buffer, 1, bytesReceivedsoFar, fp);
        
        if (bytesWritten < bytesReceivedsoFar) {
            perror("Error writing all bytes to disk (disk full?)");
        } else {
            printf("SUCCESS: Buffer successfully dumped to file. (%zu bytes written)\n", bytesWritten);
        }
        
        // 3. Always close the file stream to flush data to disk
        fclose(fp);
    }
    
    // --- END OF FILE DUMP CODE ---
    
} else {
    printf("FAILURE: File transfer incomplete. Skipping file dump.\n");

} 
*/

    /* write: echo the input string back to the client */
    /* TODO: Phase 4
    n = send(connfd, buf, strlen(buf), 0);

    if (n < 0)
      error("ERROR sending back to socket");
      */
}

int main(int argc, char **argv) {
  int listenfd; /* listening socket */
  int connfd; /* connection socket */
  //int portno; /* port to listen on */
  socklen_t clientlen ; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */

  int optval; /* flag value for setsockopt */


  /* check command line args */
  process_args(argc, argv);


  /* socket: create a socket */
  // TODO: add comments
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets
   * us rerun the server immediately after we kill it;
   * otherwise we have to wait about 20 secs.
   * Eliminates "ERROR on binding: Address already in use" error.
   */
  optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
	     (const void *)&optval , sizeof(int));

  /* build the server's internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET; /* we are using the Internet */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* accept reqs to any IP addr */
  serveraddr.sin_port = htons((unsigned short)portno); /* port to listen on */

  /* bind: associate the listening socket with a port */
  if (bind(listenfd, (struct sockaddr *) &serveraddr,
	   sizeof(serveraddr)) < 0)
    error("ERROR on binding");

  /* listen: make it a listening socket ready to accept connection requests */
  if (listen(listenfd, QSIZE) < 0) /* allow n requests to queue up */
    error("ERROR on listen");

  /* main loop: wait for a connection request, echo input line,
     then close connection. */
  clientlen = sizeof(clientaddr);
  while (1) {

    /* accept: wait for a connection request */
    connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (connfd < 0)
      error("ERROR on accept");

    process_client_request(connfd, clientaddr);

    close(connfd);
  }
}
