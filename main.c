#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define PORT "8000"
#define BACKLOG 10
#define MAXPAYLOADSIZE 500
#define MAXDATASIZE 1000
#define MAXVALUESIZE 100
#define MAXMESSAGESIZE 100

const char* server="server-name";

void logger(char* tag, char* message, char* client_ip);
void handler(int sock, char* client_ip);
int parseData(char* buffer, char* client_ip, char* dst, char* message, char* start, char* end, int offset);

int main()
{
    // listen on sock_fd, new connection on new_fd
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    // connector's address information
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int yes=1;
    char client_ip[INET6_ADDRSTRLEN];
    int rv;

    // Max message size
    char message[MAXMESSAGESIZE];
    bzero(message,MAXMESSAGESIZE);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        sprintf(message, "getaddrinfo: sss %s", gai_strerror(rv));
        logger("ERROR", message, NULL);
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            logger("INFO", "socket failed", NULL);
#ifdef DEBUG
            perror("server: socket");
#endif
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            logger("ERROR", "setsockopt failed", NULL);
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            logger("INFO", "bind: Address already in use", NULL);
#ifdef DEBUG
            perror("server: bind");
#endif
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        logger("ERROR", "bind failed", NULL);
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        logger("ERROR", "lisen failed", NULL);
#ifdef DEBUG
        perror("listen");
#endif
        exit(1);
    }

    // Set SIGCHLD handler to SIG_IGN which causes processes to be reaped automatically:
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
        logger("ERROR", "signaction error", NULL);
#ifdef DEBUG
        perror("accept");
#endif
        // exit(1);
    }

    logger("INFO", "Starting server", NULL);

    while(1)
    {
        sin_size = sizeof client_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1)
        {
            logger("INFO", "Accept error", NULL);
#ifdef DEBUG
            perror("accept");
#endif
            continue;
        }

        inet_ntop(client_addr.ss_family, &(((struct sockaddr_in*)&client_addr)->sin_addr), client_ip, sizeof client_ip);
        if (!fork())
        {
            // Close listener
            close(sockfd);
            handler(new_fd, client_ip);
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }
    return 0;
}


int parseData(char* buffer, char* client_ip, char* dst, char* message, char* start, char* end, int offset)
{
    char* p1 = strstr(buffer,start);
    if (p1 == NULL)
    {
        sprintf(message,"Input value not found");
        logger("ERROR", message, client_ip);
        return 1;
    }

    p1 = p1 + strlen(start) + offset;
    char* p2 = strstr(p1,end);
    if (p2 == NULL)
    {
        sprintf(message,"Malformed input");
        logger("ERROR", message, client_ip);
        return 1;
    }

    int n = (int) (p2 - p1);
    if (n<MAXVALUESIZE)
    {
        strncpy(dst,p1,n);
        dst[n] = '\0';
    }
    else
    {
        sprintf(message,"Max value size exceded size:%d MAXVALUESIZE: %d\n", n, MAXVALUESIZE);
        logger("ERROR", message, client_ip);
        return 1;
    }
#ifdef DEBUG
    logger("DEBUG", dst, client_ip);
#endif
    return 0;
}

/* @brief log data */
void logger(char* tag, char* message, char* client_ip)
{
    char buffer[MAXDATASIZE];
    bzero(buffer,MAXDATASIZE);

    // Get time
    time_t now;
    time(&now);
    char* ctime_str = ctime(&now);

    // Chomp line
    int l = strlen(ctime_str) - 1;
    ctime_str[l] = '\0';

    if (client_ip)
    {
        (void)snprintf(buffer,MAXDATASIZE,"[%s]\t%s\t%s\t%d\t%s\t%s", tag, ctime_str, SERVICE, getpid(), client_ip, message);
    }
    else
    {
        (void)snprintf(buffer,MAXDATASIZE,"[%s]\t%s\t%s\t%d\t%s", tag, ctime_str, SERVICE, getpid(), message);
    }

    printf("%s\n",buffer);
    fflush(stdout);
}


void handler(int sock, char* client_ip)
{
    int n1=0;
    int n2=0;
    int len=0;
    int rtn=0;
    char* matchData;
    char* matchStatus;
    int dataLen=0;
    int serverError=0;
    int clientError=0;

    // Get time
    time_t now;
    time(&now);
    char* ctime_str = ctime(&now);

    // Chomp line
    int l = strlen(ctime_str) - 1;
    ctime_str[l] = '\0';

    // Max Value1 size
    char value1[MAXVALUESIZE];
    bzero(value1,MAXVALUESIZE);

    // Max Value2 size
    char value2[MAXVALUESIZE];
    bzero(value2,MAXVALUESIZE);

    // Max URI size
    char uri[MAXVALUESIZE];
    bzero(uri,MAXVALUESIZE);

    // Max message size
    char message[MAXMESSAGESIZE];
    bzero(message,MAXMESSAGESIZE);

    // Max playload data to send
    char data[MAXPAYLOADSIZE];
    bzero(data,MAXPAYLOADSIZE);

    // Max data to transfer
    char buf[MAXDATASIZE];
    bzero(buf,MAXDATASIZE);
    if( (n1 = recv(sock, buf, MAXDATASIZE, 0)) == -1)
    {
        logger("ERROR", client_ip, "Receive failure");
        perror("recv failed");
        serverError=1;
    }

#ifdef DEBUG
    sprintf(message, "nBytes recv: %d data recv: %s", n1, buf);
    logger("DEBUG", message, client_ip);
    bzero(message,MAXMESSAGESIZE);
#endif

    // Get URI
    rtn = parseData(buf, client_ip, uri, message, "/", " HTTP/1.1", -1);
    if (rtn)
    {
        clientError=1;
    }

    // Get data payload
    char* pData = strstr(buf,"{");
    if (pData)
    {
        sprintf(message, "%s\t%s", uri, pData);
        logger("INFO", message, client_ip);
    }

    // Support endpoints
    matchData = strstr (buf,"POST /data HTTP");
    matchStatus = strstr (buf,"GET /status HTTP");


    // Search buffer for data
    if (matchData)
    {
        char* start = "\"user\"";
        char* end = "\"";
        rtn = parseData(buf, client_ip, value1, message, start, end, 3);
        if (rtn)
        {
            clientError=1;
        }

        char* start2 = "\"email\"";
        rtn = parseData(buf, client_ip, value2, message, start2, end, 3);
        if (rtn)
        {
            clientError=1;
        }
    }


    // Return data
    bzero(buf,MAXDATASIZE);
    if (serverError)
    {
        dataLen = snprintf(data,MAXPAYLOADSIZE,"{\"message\": \"Internal Server Error\",\"service\": \"%s\", \"version\": \"%s\"}\n",SERVICE, VERSION);
        /* Header + a blank line + data*/
        len = snprintf(buf,MAXDATASIZE,"HTTP/1.1 500 Internal Server Error\nContent-Type: application/json\nContent-Length: %d\nServer: %s\nDate: %s\n\n%s", dataLen, server, ctime_str, data);
        serverError=0;
    }
    else if (clientError)
    {
        dataLen = snprintf(data,MAXPAYLOADSIZE,"{\"message\": \"%s\",\"service\": \"%s\", \"version\": \"%s\"}\n",message, SERVICE, VERSION);
        /* Header + a blank line + data*/
        len = snprintf(buf,MAXDATASIZE,"HTTP/1.1 400 Bad Request\nContent-Type: application/json\nContent-Length: %d\nServer: %s\nDate: %s\n\n%s", dataLen, server, ctime_str, data);
        clientError=0;
    }
    else if (matchStatus)
    {
        dataLen = snprintf(data,MAXPAYLOADSIZE,"{\"message\": \"OK\",\"service\": \"%s\", \"version\": \"%s\"}\n",SERVICE, VERSION);
        /* Header + a blank line + data*/
        len = snprintf(buf,MAXDATASIZE,"HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %d\nServer: %s\nDate: %s\n\n%s", dataLen, server, ctime_str, data);
    }
    else if (matchData)
    {
        dataLen = snprintf(data,MAXPAYLOADSIZE,"{\"message\": \"OK\",\"user\": \"%s\",\"email\": \"%s\",\"service\": \"%s\", \"version\": \"%s\"}\n",value1,value2,SERVICE, VERSION);
        /* Header + a blank line + data*/
        len = snprintf(buf,MAXDATASIZE,"HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %d\nServer: %s\nDate: %s\n\n%s", dataLen, server, ctime_str, data);
    }
    else
    {
        dataLen = snprintf(data,MAXPAYLOADSIZE,"{\"message\": \"Bad Request\",\"service\": \"%s\", \"version\": \"%s\"}\n",SERVICE, VERSION);
        /* Header + a blank line + data*/
        len = snprintf(buf,MAXDATASIZE,"HTTP/1.1 400 Bad Request\nContent-Type: application/json\nContent-Length: %d\nServer: %s\nDate: %s\n\n%s", dataLen, server, ctime_str, data);
    }


    // Send data
    if ((n2 = send(sock, buf, len, 0)) == -1)
    {
        sprintf(message, "%s\tSending data failed", uri);
        logger("ERROR", message, client_ip);
        perror("send");
    }

    sprintf(message, "%s\t%s", uri, data);
    logger("INFO", message, client_ip);
#ifdef DEBUG
    sprintf(message, "%s\tnBytes sent: %d data sent: %s",uri, n2, buf);
    logger("DEBUG", message, client_ip);
    bzero(message,MAXMESSAGESIZE);
#endif
}
