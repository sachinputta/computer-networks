#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// HTTP 1.0 status lines
#define HTTP_OK "HTTP/1.0 200 OK\n"
#define HTTP_404 "HTTP/1.0 404 Not Found\n"
#define HTTP_403 "HTTP/1.0 403 Forbidden\n"
#define HTTP_400 "HTTP/1.0 400 Bad Request\n"

// HTTP 1.1 status lines
#define HTTP_11_OK "HTTP/1.1 200 OK\n"
#define HTTP_11_404 "HTTP/1.1 404 Not Found\n"
#define HTTP_11_403 "HTTP/1.1 403 Forbidden\n"
#define HTTP_11_400 "HTTP/1.1 400 Bad Request\n"


struct thread_arg{
  int connection;
  struct sockaddr_in client;
};

void *handleConnection(void*);

int main(int argc, char ** argv) {

  int fd, nread;
  char buf[1024];
  struct sockaddr_in myaddr, cliaddr;
  int sock;
  socklen_t len;

  // port_no=2222
  myaddr.sin_port = htons(2222);
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  if(bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr))< 0) {
    printf("error binding socket");
    close(sock);
    return -1;
  }
  
  if(listen(sock, 15) < 0) {
    printf("error listening to socket");
    close(sock);
    return -1;
  }
int pid ;
pid=fork();


  // blocking accept function
  
  if(pid==0){
    sleep(5);
  len = sizeof(cliaddr);
  pthread_t tid;
  while(1) {
    int connection = accept(sock, (struct sockaddr *) &cliaddr, &len);
    if(connection == -1){
      printf("Connection failed");
    }
    struct thread_arg* arg = malloc(sizeof(struct thread_arg));
    arg->connection = connection;
    arg->client = cliaddr;
    int thread = pthread_create(&tid, NULL, handleConnection, arg);
  } 
  close(sock);
  }

  return 0;
}



void sendResponse(int conn, int fd, char *http_response, int response_size, char * fileType) {
  // http response header
  write(conn, http_response, response_size);

  // Date
  char date_buf[500];
  time_t now = time(0);
  struct tm tm =  *gmtime(&now);
  strftime(date_buf, sizeof(date_buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
  write(conn, "Date: ", strlen("Date: "));
  write(conn, date_buf, strlen(date_buf));
  write(conn, "\n", strlen("\n"));
       
  if(fd == -1) {
    // there is no content-type 
    
    // Content length is zero
    write(conn, "Content-Length: 0\r\n", strlen("Content-Length: 0\r\n")); 
    
  } else {
    // we have valid content to write
    int nread;
    int buffer[1024];
    int size = 0;
    while((nread = read(fd, buffer, sizeof(buffer))) > 0) {
      size += nread;
    } 
    char fileBuffer[size];
    pread(fd, fileBuffer, size, 0);
    write(conn, "Content-Type: ", strlen("Content-Type: "));
    if(strcmp(fileType, "html") == 0) {
      write(conn, "text/html\n", strlen("text/html\n"));
    } else if (strcmp(fileType, "txt") == 0) {
      write(conn, "text/plain\n", strlen("text/plain\n"));
    } else if (strcmp(fileType, "jpg") == 0 || strcmp(fileType, "jpeg") == 0) {
      write(conn, "image/jpg\n", strlen("image/jpg\n"));
    } else if (strcmp(fileType, "gif") == 0) {
      write(conn, "image/gif\n", strlen("image/gif\n"));
    } else {
      write(conn, "unknown\n", strlen("unknown\n"));
    } 
    write(conn, "Content-Length: ", strlen("Content-Length: "));
    char size_buf[50];
    sprintf(size_buf,"%d\n\n", size);
    write(conn,size_buf,strlen(size_buf));
    write(conn,fileBuffer,size);
  }
}

void * handleConnection(void * arg) {
  // variables
  struct thread_arg* args = arg;
  int connection = args->connection;
  struct sockaddr_in cliaddr = args->client;
  char* buffer = malloc(1024);
  char* token;
  int nread, fd;
  int keep_alive = 1;

  // notify console of connection
  printf("Connection from %s, port %d:\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

  while(keep_alive) {
    if((nread = read(connection, buffer, 1024)) > 0){
      // write contents to buffer for debug
      write(1, buffer, nread);
      
      
      // first token is request type
      token = strtok(buffer, " \n\r");
      if(strcmp(token, "GET") != 0) {
	sendResponse(connection, -1, HTTP_11_400, strlen(HTTP_11_400), NULL);
	break;
      }
      char *request_type = token;
 
      // second token is url
      token = strtok(NULL, " \n\r");
      if(strstr(token, "../") != NULL) {
	sendResponse(connection, -1, HTTP_11_403, strlen(HTTP_11_403), NULL);
	break;
      }
      char *request_url = token;

      // third token is HTTP protocol
      token = strtok(NULL, " \n\r");
      if(strcmp(token, "HTTP/1.0") != 0 && strcmp(token, "HTTP/1.1") != 0) {
	sendResponse(connection, -1, HTTP_11_400, strlen(HTTP_11_400), NULL);
	break;
      }
      char *protocol = token;

      printf("filename: %s\n", request_url);
      if(strcmp(request_url, "/") == 0) {
	request_url = "page.html";
      }
      if(*request_url == '/') {
	request_url = request_url++;
      }
      char* fileType = strrchr(request_url, '.') + sizeof(char);
      fd = open(request_url, O_RDONLY);
      if(strcmp(protocol, "HTTP/1.0") == 0) {
	// HTTP 1.0 protocol
	if(fd == -1) {
	  sendResponse(connection, fd, HTTP_404, strlen(HTTP_404), fileType);
	} else {
	  sendResponse(connection, fd, HTTP_OK, strlen(HTTP_OK), fileType);
	}
	// HTTP 1.1 does not keep connection alive
	break; 
      } else {
	// HTTP 1.1 protocol 
	if(fd == -1) {
	  sendResponse(connection, fd, HTTP_11_404, strlen(HTTP_11_404), fileType);
	  break;
	} else {
	  sendResponse(connection, fd, HTTP_11_OK, strlen(HTTP_11_OK), fileType); 
	}
      }
    }
    
  }
  
  close(connection);

  free(buffer);
  free(arg);
  return(NULL);
}
