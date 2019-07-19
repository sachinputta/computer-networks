#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
struct row{
	char name[256];
	char value[256];
};
void new_sent_four(int clientsocket){
	char wrong[256];
	strcpy(wrong,"entry not in the database");
	if(send(clientsocket, wrong, 256, 0)<0){
		perror("Error in sending\n");
		exit(1);
	}
}

int main(int argc, char *argv[]){
	int sockid, statusb, statusl, clientLen, clientsocket, recvMsgSize, port, count;
	struct sockaddr_in addrport, clientAddr;
	char buffer[256];
	port = atoi(argv[1]);
  
    char rbuffer[100],lbuffer[100];

	if((sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){		
		perror("Socket creation failed\n");
		exit(1);
	}

	
	addrport.sin_family = AF_INET;
	addrport.sin_port = htons(port);	
	addrport.sin_addr.s_addr = htonl(INADDR_ANY);	


	if((statusb = bind(sockid, (struct sockaddr *)&addrport, sizeof(addrport)))<0){
		perror("Error binding the socket\n");
		exit(1);
	}


	if((statusl = listen(sockid, 12))<0)
	{	
		perror(" Failed listen()\n");
		exit(1);
	}


	while(1){
		clientLen = sizeof(clientAddr);
		if((clientsocket = accept(sockid, (struct sockaddr *)&clientAddr, &clientLen))<0){
			perror("failed accept()\n");
			exit(1);
		}
		printf("Connection established with Client\n");
		if ((recvMsgSize = recv(clientsocket, buffer, 256, 0)) < 0){
			perror("failed recv()\n");
			exit(1);
		}
		int i, j, flag;
		
		
		char type, value[256];
		type=buffer[0];
		for(i=1; i<strlen(buffer)-1;i++){	
			value[i-1]=buffer[i];
		}
		value[i-1]='\0';
		printf("Message Received\nRequest_Type : %c, Message:%s\n", type, value);

		FILE *fp;
		fp = fopen("one.txt", "r");
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		char query[256], result[256];
		flag=0;
		if(type=='1'){
			while (fscanf(fp,"%s%s",rbuffer,lbuffer)==2) {
                
                
				if(strcmp(rbuffer, value)==0){
					 					printf("Query request successful and ...... Found : %s\n", lbuffer);
					if((count = send(clientsocket, lbuffer, 256, 0))<0){
							perror("Error in sending..\n");
							exit(1);
					}
					close(clientsocket);
					flag=1;
					break;
				}
			}
		}
		
		if(flag==0){
			printf("Query request unsuccessful.. NOT FOUND\n");
			new_sent_four(clientsocket);
		}
		printf("Connection Closed with Client\n");
	}
	return 0;
}
