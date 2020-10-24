#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT_NO 3205 //Defining our port number.
void * receive(); //Defining functions and variables.
int privateOrNot;

int main(){
	//Defining address, port and sockets.
	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NO);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");
  

	//if connected creating our client thread.
	pthread_t thread;
	pthread_create(&thread, NULL, receive, (void *) &clientSocket );
  	char nickname[15];//getting our nickname by scanf.
  	printf("Enter your nickname: ");
  	scanf("%s", nickname);
	send(clientSocket,nickname,15,0);//and send this nickname to the server so that it can create a new client with that nickname.

	while(1){//infinite loop so that it will always get commands.

		char input[1024];
		scanf("%s",input);

		if(strcmp(input,"-list") == 0){

			send(clientSocket,input,1024,0); //send command to the server
			
		}
		if(strcmp(input,"-msg") == 0){
      
			send(clientSocket,input,1024,0);  // send command to server
			fgets(input, 1024, stdin);
			send(clientSocket,input,1024,0);	// send message to server
			

		}
		if(strcmp(input,"-enter") == 0){
      
			send(clientSocket,input,1024,0); //send command to the server
			
			scanf("%s",input);
			send(clientSocket,input,1024,0); //send which room he/she wants to enter to the server

            char learn[1024];
		    int read = recv(clientSocket,learn,1024,0);//check if this room is private or public.
		    learn[read] = '\0';
            if(strcmp(learn, "Private") == 0){
                privateOrNot = 1;
            }
            else
            {
                privateOrNot = 0;
            }
            
            if(privateOrNot == 1){//if this room is private, then ask user for password.
                char pass[10];
				printf("Please enter room's password: \n");
			    scanf("%s", pass);
                strcat(pass, "\0");
			    send(clientSocket,pass,1024,0);//send this password to server.
                privateOrNot = 0;
            }



		}
		if(strcmp(input,"-create") == 0){
      
			send(clientSocket,input,1024,0);//send command to the server.
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);//send room name to the server.
			

		}
		if(strcmp(input,"-pcreate") == 0){

			send(clientSocket,input,1024,0);//send command to the server.

			scanf("%s",input);
			send(clientSocket,input,1024,0);//send private room's name to the server.

			char password[10];
  			printf("Define a password for your private room: \n");//ask user to define a password for room.
 			scanf("%s", password);

  
  			send(clientSocket,password,10,0);//send this password to the server.
			

		}
		if(strcmp(input,"-quit") == 0){
      
			send(clientSocket,input,1024,0);//send command to the server.
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);//send room to quit to the server.
			

		}
		if(strcmp(input,"-whoami") == 0){
      
			send(clientSocket,input,1024,0);//send command to the server.

		}
		if(strcmp(input,"-exit") == 0){
      
			send(clientSocket,input,1024,0);//send command to the server and also do exit(0).
			exit(0);

		}
		
	}


}


void * receive(void * sockID){// receiving thread for it to constantly receiving.

	int clientSocket = *((int *) sockID);

	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);//receive the data, and below print it if it's not the information about the room(almost everything).
		data[read] = '\0';
        if(strcmp(data, "Private") == 0){
            privateOrNot = 1;
        }
        else{
		    printf("%s\n",data);
        }
		

	}

}