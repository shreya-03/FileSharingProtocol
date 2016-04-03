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

#define LENGTH 10024 

void stcat(char *str1, char *str2){
	int i = 0,len = 0;
	while(*(str1+len)!='\0')
		len++;
	while(*(str2+i)!='\0')
	{
		*(str1+len) = *(str2+i);
		i++;
		len++;
	}
	*(str1+len) = '\0';
}

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	/* Variable Definition */
	int sockfd; 
	int nsockfd;
	int udpsockfd;
	int nudpsockfd;
	struct sockaddr_in remote_addr;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	char revbuf[LENGTH]; 
	int PORT;
	char *IPA;
	int n;
	int nBytes;
	char buffer[10024];
	char option[10024];
	/* Get the Socket file descriptor */
	if(argc != 3){
		fprintf(stderr,"Invalid no. of arguments\n");
		exit(1);
	}
	IPA = argv[1];
	PORT = atoi(argv[2]);
	/* Fill the socket address struct */


	int success = 0;
	while(success == 0){
		bzero(option,10024);
		//fgets(option,10023,stdin);
		//scanf("%s",option);
		//scanf("%[^\n]s",option);
		if ((udpsockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to obtain (UDP) Socket Descriptor! (errno = %d)\n",errno);
			exit(1);
		}

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
			exit(1);
		}


		serverAddr.sin_family = AF_INET; 
		serverAddr.sin_port = htons(PORT); 
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		bzero(&(serverAddr.sin_zero), 8);

		remote_addr.sin_family = AF_INET; 
		remote_addr.sin_port = htons(PORT); 
		inet_pton(AF_INET, IPA, &remote_addr.sin_addr); 
		bzero(&(remote_addr.sin_zero), 8);
		if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
			exit(1);
		}
		else 
			printf("[Client] Connected to server at port %d...ok!\n", PORT);


		bzero(&(remote_addr.sin_zero), 8);

		addr_size = sizeof(serverAddr);
		fgets(option,10024,stdin);
		char check[10024];
		strcpy(check,option);
		if(strstr(option,"exit")){
			success = 1;
			n = write(sockfd,"exit",strlen("exit"));
		}

		if(success == 0){
			char *y = strtok(check," ");
			if(strstr(y,"FileDownload")){
				y = strtok(0," ");
				y = strtok(0," ");
			}
			if(strstr(y,"UDP")){
				n = write(sockfd,"U",strlen("U"));
				sendto(udpsockfd,option,strlen(option),0,(struct sockaddr *)&serverAddr,addr_size);
			}
			else
				n = write(sockfd,option,strlen(option));
		}
		char *x;
		x = strtok(option," ");
		//printf("x: %s\n",x);
		if(strstr(option,"IndexGet")){
			char *flag;
			char command[10024];
			flag = strtok(0," ");
			if(strstr(flag,"longlist")){
				char buf1[10024];
				bzero(buf1,10024);
				n = read(sockfd,buf1,10023);
				printf("%s\n",buf1);
			}
			else if(strstr(flag,"shortlist")){
				char buf1[10024];
				bzero(buf1,10024);
				n = read(sockfd,buf1,10023);
				printf("%s\n",buf1);
			}
			else if(strstr(flag,"regex")){
				char buf1[10024];
				bzero(buf1,10024);
				n = read(sockfd,buf1,10023);
				printf("%s\n",buf1);
			}
		}

		else if(strstr(option,"FileHash")){
			char *flag;
			char command[10024];
			flag = strtok(0," ");
			if(strstr(flag,"verify")){
				char buf1[10024];
				bzero(buf1,10024);
				n = read(sockfd,buf1,10023);
				printf("%s\n",buf1);
			}
			else if(strstr(flag,"checkall")){
				char final[10024];
				bzero(final,10024);
				n = read(sockfd,final,10023);
				printf("%s\n",final);
			}
		}

		else if(strstr(option,"FileUpload")){	
			/* Send File to Server */

			char fs_name[100];
			char *sname;
			char command[200];
			char host[100];
			getlogin_r(host,99);
			strcpy(fs_name,"/home/");
			strcat(fs_name,host);
			strcat(fs_name,"/client_files/");
			//strcpy(fs_name,"/home/manasa/client_files/");
			//char sname[100];
			//scanf("%s",sname);
			sname = strtok(0," ");
			strcat(fs_name,sname);
			//bzero(sname,10024);
			//n = write(sockfd,sname,strlen(sname));
			char *type = strtok(0," ");
			char sdbuf[LENGTH]; 
			printf("[Client] Sending %s to the Server... ", fs_name);
			FILE *fs = fopen(fs_name, "r");
			if(fs == NULL)
			{
				printf("ERROR: File %s not found.\n", fs_name);
				exit(1);
			}

			bzero(sdbuf, LENGTH); 
			int fs_block_sz; 
			while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
			{
				if(send(sockfd, sdbuf, fs_block_sz, 0) < 0)
				{
					fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
					break;
				}
				bzero(sdbuf, LENGTH);
			}
			printf("Ok File %s from Client was Sent!\n", fs_name);
			sprintf(command,"ls -l %s | awk '{print $9,$5,$8}'; md5sum %s; ",fs_name,fs_name);
			system(command);
		}
		else if(strstr(option,"FileDownload")){
			/* Receive File from Server */
			char command[100];
			char final[10024];
			printf("[Client] Receiving file from Server and saving it ...\n");
			char fr_name[10024];
			char sr_name[10024];
			//strcpy(fr_name,"/home/manasa/client_files/");
			//strcpy(sr_name,"/home/manasa/server_files/");
			char host[100];
			getlogin_r(host,99);
			strcpy(fr_name,"/home/");
			strcat(fr_name,host);
			strcat(fr_name,"/client_files/");
			strcpy(sr_name,"/home/");
			strcat(sr_name,host);
			strcat(sr_name,"/server_files/");
			//char sname1[10024];
			//scanf("%s",sname1);
			char *sname1;
			sname1 = strtok(0," ");
			strcat(fr_name,sname1);
			strcat(sr_name,sname1);
			//bzero(sname1,10024);
			//n = write(sockfd,sname1,strlen(sname1));
			char *type;
			type = strtok(0," ");
			if(strstr(type,"TCP")){
				FILE *fr = fopen(fr_name, "w+");
				if(fr == NULL)
					printf("File %s Cannot be opened.\n", fr_name);
				bzero(revbuf, LENGTH); 
				int fr_block_sz = 0;
				while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
				{
					int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
					if(write_sz < fr_block_sz)
					{
						error("File write failed.\n");
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
					}
				}
				printf("Ok received from server!\n");
				//printf("%s\n",fr_name);
				fclose(fr);
				bzero(final,10024);
				n = read(sockfd,final,10023);
				printf("%s\n",final);
			}
			else if(strstr(type,"UDP")){
				FILE *fr = fopen(fr_name, "w+");
				if(fr == NULL)
					printf("File %s Cannot be opened.\n", fr_name);
				bzero(revbuf, LENGTH); 
				int fr_block_sz = 0;
				while((fr_block_sz = recvfrom(udpsockfd, revbuf, LENGTH, 0, (struct sockaddr *)&serverAddr, &addr_size)) > 0)
				{
					//printf("%s\n",revbuf);
					int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
					if(write_sz < fr_block_sz)
					{
						error("File write failed.\n");
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
						fprintf(stderr, "recvfrom() failed due to errno = %d\n", errno);
					}
				}
				printf("Ok received from server!\n");
				//printf("%s\n",fr_name);
				fclose(fr);
				bzero(final,10024);
				n = read(sockfd,final,10023);
				printf("%s\n",final);
			}
		}

		else if(strstr(option,"exit")){
			success = 1;
		}
	}
	printf("[Client] Connection lost.\n");
	close (sockfd);
	//return (0);
}
