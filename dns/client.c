#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
   int so_fd, port_no, i;
   struct sockaddr_in ser_add;
   struct hostent *server;
   char buff[256];
	port_no = atoi(argv[1]);

   /* Create a socket point */
   so_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (so_fd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   bzero((char *) &ser_add, sizeof(ser_add));
   ser_add.sin_family = AF_INET;
    ser_add.sin_addr.s_addr=INADDR_ANY;
   ser_add.sin_port = htons(port_no);
   
   /* Now connect to the server */
   if (connect(so_fd, (struct sockaddr*)&ser_add, sizeof(ser_add)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   printf("Plz... enter the message: ");
   bzero(buff,256);
   fgets(buff,255,stdin);
   /* Send message to the server */
   i = send(so_fd, buff, 256, 0);
   // i = write(sockfd, buffer, strlen(buffer));
   
   if (i < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   /* Now read server response */
   bzero(buff,256);
   i = read(so_fd, buff, 255);
   
   if (i < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   printf("%s\n",buff);
   close(so_fd);
   return 0;
}
