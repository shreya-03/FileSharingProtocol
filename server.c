#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


#define BACKLOG 5
#define LENGTH 10024 

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	/* Defining Variables */
	int sockfd;
	int udpsockfd;
	int nsockfd; 
	int nudpsockfd;
	int optionfd;
	int num;
	int n;
	int nBytes;
	int sin_size;
	int PORT;
	struct sockaddr_in addr_local; /* client addr */
	struct sockaddr_in addr_remote; /* server addr */
	struct sockaddr_in clientAddr; /* client addr */
	struct sockaddr_in serverAddr; /* server addr */
	struct sockaddr_storage serverStorage;
	char revbuf[LENGTH]; // Receiver buffer
	char option[10024];
	char buffer[10024];
	socklen_t addr_size,client_addr_size;	
	/* Get the Socket file descriptor */

	if(argc != 2){
		fprintf(stderr, "Invalid no. of arguments\n");
		exit(1);
	}
	PORT=atoi(argv[1]);

	if((udpsockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain (UDP) Socket Descriptor. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Obtaining (UDP) socket descriptor successfully.\n");

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Obtaining socket descriptor successfully.\n");


	/* Fill the client socket address struct */
	serverAddr.sin_family = AF_INET; // Protocol Family
	serverAddr.sin_port = htons(PORT); // Port number
	serverAddr.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	bzero(&(serverAddr.sin_zero), 8); // Flush the rest of struct

	addr_local.sin_family = AF_INET; // Protocol Family
	addr_local.sin_port = htons(PORT); // Port number
	addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */

	if( bind(udpsockfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind Port(UDP). (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully(UDP).\n",PORT);


	if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);

	/* Listen remote connect/calling */

	addr_size = sizeof(serverStorage);

	if(listen(sockfd,BACKLOG) == -1)
	{
		fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
		exit(1);
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT);

	int success = 0;
	char *op;
	while(success == 0)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1) 
		{
			fprintf(stderr, "ERROR: Obtaining new Socket Descriptor. (errno = %d)\n", errno);
			exit(1);
		}
		else 
			printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));




		bzero(option,10024);
		n=0;
		n = read(nsockfd,option,10023);
		if(strstr(option,"exit")){
			continue;
		}
		if(strcmp(option,"U")==0){
			strcpy(option,"");
			n = recvfrom(udpsockfd,option,10024,0,(struct sockaddr *)&serverStorage,&addr_size);
			printf("Sent: %s",option);
		}
		//printf("%s\n",option);
		/*Receive File from Client */
		op = strtok(option," ");
		if(strstr(op,"IndexGet")){
			char command[10024];
			char flag[10024];	
			char *st,*et;
			char *reg;
			char *fl;
			//struct hostent *hp;
			//hp = gethostbyname("127.0.0.1");
			char host[100];
			//host = (char *)hp;
			getlogin_r(host,99);
			int shour,smin,ehour,emin;
			fl = strtok(0," ");
			if(strstr(fl,"longlist")){
				char buf1[10024];
				char final[10024];
				strcpy(final,"");
				strcpy(buf1,"");
				FILE *ptr;
				sprintf(command,"ls -l /home/%s/server_files | awk '{print $9,$5,$8,$1;}' ",host);
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
			}
			else if(strstr(fl,"shortlist")){

				char buf1[10024];
				char final[10024];
				strcpy(final,"");
				FILE *ptr;
				st = strtok(0," ");
				et = strtok(0," ");

				shour = (st[0]-'0')*10 + (st[1] - '0');
				ehour = (et[0]-'0')*10 + (et[1] - '0');
				smin = (st[3]-'0')*10 + (st[4] - '0');
				emin = (et[3]-'0')*10 + (et[4] - '0');

				sprintf(command, "bash script.sh %d %d %d %d", shour,ehour,smin,emin);
				//system(command);	
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
			}
			else if(strstr(fl,"regex")){
				//scanf("%s",reg);
				reg = strtok(0," ");
				char buf1[10024];
				char final[10024];
				strcpy(final,"");
				strcpy(buf1,"");
				FILE *ptr;
				sprintf(command,"ls -l /home/%s/server_files | awk '{print $9,$5,$8,$1;}' | grep %s",host,reg);
				//system(command);	
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
			}
			else{
				printf("Invalid command");
			}
		}

		else if(strstr(op,"FileHash")){
			char *fl;
			char *f_arg;
			char path[100];
			char command[100];
			//scanf("%s",flag);
			fl = strtok(0," ");

			if(strstr(fl,"verify")){
				//scanf("%s",f_arg);
				char buf1[10024];
				char final[10024];
				strcpy(buf1,"");
				strcpy(final,"");
				FILE *ptr;
				strcpy(path,"/home/manasa/server_files/");
				f_arg = strtok(0," ");
				strcat(path,f_arg);
				sprintf(command, "md5sum %s; ls -l %s | awk '{print $8}' ", path, path);
				//system(command);
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));

			}
			else if(strstr(fl,"checkall")){
				char buf1[10024];
				FILE *ptr;
				char final[10024];
				strcpy(buf1,"");
				strcpy(final,"");
				sprintf(command, "bash script_checkall.sh");
				//system(command);	
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
			}
			else{
				printf("Invalid command");
			}


		}

		else if(strstr(op,"FileUpload")){
			char fr_name[100];
			strcpy(fr_name,"/home/manasa/server_files/");
			char *sname;
			//bzero(sname,10024);
			//n = read(nsockfd,sname,10023);
			sname = strtok(0," ");
			strcat(fr_name,sname);
			char *type = strtok(0," ");
			FILE *fr = fopen(fr_name, "w+");
			if(fr == NULL)
				printf("File %s Cannot be opened file on server.\n", fr_name);
			else
			{
					bzero(revbuf, LENGTH); 
					int fr_block_sz = 0;
					while((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0) 
					{
						int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
						if(write_sz < fr_block_sz)
						{
							error("File write failed on server.\n");
						}
						bzero(revbuf, LENGTH);
						if (fr_block_sz == 0 || fr_block_sz != 512) 
						{
							break;
						}
					}
					if(fr_block_sz < 0)
					{
						if (errno == EAGAIN)
						{
							printf("recv() timed out.\n");
						}
						else
						{
							fprintf(stderr, "recv() failed due to errno = %d\n", errno);
							exit(1);
						}
					}
					printf("Ok received from client!\n");
					fclose(fr); 

			}
		}

		else if(strstr(op,"FileDownload")){
			/* Send File to Client */
			char fs_name[10024];
			char command[10024];
			strcpy(fs_name,"/home/manasa/server_files/");
			char *sname1;
			char buf1[10024];
			FILE *ptr;
			char final[10024];
			strcpy(buf1,"");
			strcpy(final,"");
			//bzero(sname1,10024);
			//n = read(nsockfd,sname1,10023);
			sname1 = strtok(0," ");
			char *type;
			type = strtok(0," ");
			strcat(fs_name,sname1);
			if(strstr(type,"TCP")){
				char sdbuf[LENGTH]; // Send buffer
				printf("[Server] Sending %s to the Client...", fs_name);
				FILE *fs = fopen(fs_name, "r");
				if(fs == NULL)
				{
					fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", fs_name, errno);
					exit(1);
				}

				bzero(sdbuf, LENGTH); 
				int fs_block_sz; 
				while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
				{
					if(send(nsockfd, sdbuf, fs_block_sz, 0) < 0)
					{
						fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
						exit(1);
					}
					bzero(sdbuf, LENGTH);
				}
				printf("Ok sent to client!\n");
				sprintf(command,"ls -l %s | awk '{print $9,$5,$8}'; md5sum %s; ",fs_name,fs_name);
				bzero(buf1,10024);
				bzero(final,10024);
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
				//system(command);
			}
			else if(strstr(type,"UDP")){
				char sdbuf[LENGTH]; // Send buffer
				printf("[Server] Sending %s to the Client...", fs_name);
				FILE *fs = fopen(fs_name, "r");
				if(fs == NULL)
				{
					fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", fs_name, errno);
					exit(1);
				}

				bzero(sdbuf, LENGTH); 
				int fs_block_sz; 
				while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
				{
					if(sendto(udpsockfd, sdbuf, fs_block_sz, 0,(struct sockaddr *)&serverStorage,sizeof(serverStorage)) < 0)
					{
						fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
						exit(1);
					}
					bzero(sdbuf, LENGTH);
				}
				printf("Ok sent to client!\n");
				sprintf(command,"ls -l %s | awk '{print $9,$5,$8}'; md5sum %s; ",fs_name,fs_name);
				bzero(buf1,10024);
				bzero(final,10024);
				if((ptr = popen(command,"r"))!=NULL){
					while (fgets(buf1, 10024, ptr) != NULL){
						strcat(final,buf1);
					}
					pclose(ptr);
				}
				n = write(nsockfd,final,strlen(final));
				//system(command);
			}
		}
		else if(strstr(op,"exit")){
			;
		}
		else{
			printf("Invalid command");
		}
		printf("[Server] Connection with Client closed. Server will wait now...\n");
		while(waitpid(-1, NULL, WNOHANG) > 0);
	}
}
