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

#define PORT_NO 3205 //port number
#define ROOM_NO 3 // room number
#define MAX_CLIENT_NO_IN_A_ROOM 3// maximum client in a room
#define PASSWORD_LIMIT 10// maximum password length
#define NICKNAME_LIMIT 15 // maximum nickname length
#define CLIENT_NO 1024 // client number


int clientCount = 0;// total client count.
int roomCount = 0;// total room count.


struct client{//client struct

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
    char structName[NICKNAME_LIMIT]; // nickname	
	int isInRoom;//boolean to check if client is in a room or not.
};

struct room{//room struct

	int index;
	char roomName[20];//room's name
	struct client clients[MAX_CLIENT_NO_IN_A_ROOM];//client array in room
	int isPrivate;//check the room if it's private or public.
	int roomClientCount;//client count in a room.
	char password[PASSWORD_LIMIT];// room's password.
};

struct client Client[CLIENT_NO];//Client array.
struct room Rooms[ROOM_NO];//Room array.
pthread_t thread[CLIENT_NO];
struct client empty = {0};//empty client
struct client tempClient = {0};//temp client
struct room emptyRoom = {0};//empty room


char * printList(){//List function:
    char x[5];
	int count;
	char notify[100];
    strcpy(notify, "");
	char message[15];
	strcpy(message, "Rooms: \n");
	strcat(notify, message);

    for (size_t i = 0; i < roomCount; i++)//just travelling all the rooms and printing them.
			{
				
				strcat(notify, Rooms[i].roomName);
				strcat(notify, "->>>> ");

				if(Rooms[i].isPrivate != 1){
					for (size_t j = 0; j < Rooms[i].roomClientCount; j++)
					{
						strcat(notify,Rooms[i].clients[j].structName);
						
						if(j != Rooms[i].roomClientCount-1){
							strcat(notify, ", ");
						}
					
					
					}
				}
				else{
					strcat(notify, "You can not see inside of it. Room is private.");
				}
				count = Rooms[i].roomClientCount;
				sprintf(x, "%d", count);
				strcat(notify, "   ");
				strcat(notify, x);
				strcat(notify, " person is/are in this room.");
				strcat(notify, "\n\0");
				
				
			}
			
			
			char *final = &notify[0];
			return final;
}

int findRoom(int index){//finding the person who is in the room and returning his current room number.

    int curRoomNum = -1;
    for (size_t i = 0; i < roomCount; i++)
				{
				
					for (size_t j = 0; j < Rooms[i].roomClientCount; j++)
					{
						
						if(strcmp((Rooms[i].clients[j].structName), Client[index].structName) == 0){//if the names are equal then we found the person.
							
							curRoomNum = i;				
							return curRoomNum;
						}
					
					}
				}

                return curRoomNum;
}

int findRoomByName(char *tmpRoomName){//Find the room by it's room name. Returns the position in room array.
    int currentRoom = -1;
			for (size_t i = 0; i < roomCount; i++)
			{
				if(strcmp(Rooms[i].roomName, tmpRoomName) == 0){//if room names are match, then we found our room.
					currentRoom = Rooms[i].index;
					break;
				}
			}
            return currentRoom;
}

char *createRoom(int index, int clientSocket){//Creating public room.
                char data[1024];
				char notify[100];
    			// room initialization part. I take the room name and assign it to temp room name. 
				int read = recv(clientSocket,data,1024,0);
				data[read] = '\0';
				char tmpRoomName[20];
				strcpy(tmpRoomName, data);
				int isFound = findRoomByName(tmpRoomName);//calling findroomname function
				if(isFound != -1 || roomCount >= ROOM_NO){//if there's a room with the same name, or the maximum amaount of room exists, then this warning pops up.
					
					strcpy(notify, "There is a room with the same name, choose different name or there are more than 3 rooms already!");
				}
				else//if not,
				{
					//initalizing room's count, name, private or public boolean, and it's client count.
					Rooms[roomCount].index = roomCount;
					strcpy(Rooms[roomCount].roomName, tmpRoomName);
					Rooms[roomCount].isPrivate = 0;
					Rooms[roomCount].roomClientCount = 0;

					char holder [20];
					strcpy(holder, Rooms[roomCount].roomName);
            	    strcpy(notify, "");
					strcat(notify,("Room "));
					strcat(notify,holder);
					strcat(notify ," is created. \n\0");

				// client assignment and counter incrementation
					Rooms[roomCount].clients[0] = Client[index];
					Client[index].isInRoom = 1;
					Rooms[roomCount].roomClientCount++;
					roomCount++;
				}
				
				

                char *final = &notify[0];

            return final;//returns the string that should be printed.
}

char *createPRoom(int index, int clientSocket){//creating private room. Almost same with the create room.
				//receiving room name, assinging to temp room name, and initiliaze it.
				char data[1024];
                int read = recv(clientSocket,data,1024,0);
				data[read] = '\0';
				char tmpRoomName[20];
				strcpy(tmpRoomName, data);
				Rooms[roomCount].index = roomCount;
				strcpy(Rooms[roomCount].roomName, tmpRoomName);
				Rooms[roomCount].isPrivate = 1;
				Rooms[roomCount].roomClientCount = 0;

				char holder [20];
				strcpy(holder, Rooms[roomCount].roomName);
				char notify[100];
                strcpy(notify, "");
				strcat(notify,("Private Room "));
				strcat(notify,holder);
				strcat(notify ," is created.\n\0");

			// client assignment and counter incrementation
				Rooms[roomCount].clients[0] = Client[index];
				Client[index].isInRoom = 1;
				Rooms[roomCount].roomClientCount++;
				roomCount++;


            char *final = &notify[0];
            return final;
}

char *whoami(char *nickname){//who am i function. Just prints the user's nickname.
            char final_message[100];
            strcpy(final_message, "");
			strcat(final_message, "->>> ");
            strcat(final_message, nickname);
			strcat(final_message, " <<<-\n\0");


            char *final = &final_message[0];
            return final;
}

char *quit(int index, int currentRoom){//Quit function.
    			char holder[20];
                int curClient = 0;
				strcpy(holder, Client[index].structName);
				char notify[100];
                strcpy(notify, "");
				strcat(notify,("User "));
				strcat(notify,holder);
				strcat(notify ," left the room.\n\0");
                for (size_t i = 0; i < Rooms[currentRoom].roomClientCount; i++)
                {
                    if(Client[index].index == Rooms[currentRoom].clients[i].index){
                        curClient = i;//find current client's position in rooms array
                        break;
                    }
                }

                Rooms[currentRoom].clients[curClient] = empty;// bütün elemanları çıkan elemanın yerine kadar bir kaydır
                for (size_t j = curClient; j < Rooms[currentRoom].roomClientCount-curClient; j++)
                {
                    if(j == Rooms[currentRoom].roomClientCount - 1){
                        break;
                    }
                    Rooms[currentRoom].clients[j] = Rooms[currentRoom].clients[j+1];
                }
        
                Rooms[currentRoom].clients[Rooms[currentRoom].roomClientCount-1] = empty;// son elemanı empty yap ve client count sayısını bir azalt.
                Rooms[currentRoom].roomClientCount--;
				Client[index].isInRoom = 0;//client'ın odada olma booleanını değiştir.

				if((Rooms[currentRoom].roomClientCount) == 0){//eğer odadaki son insansa bulunduğu odayı da sil. Ama bunu yapmak için odaları birer kaydırıp son elemanı silip oda countunu azaltmak gerekiyor.
                    Rooms[currentRoom] = emptyRoom;
                    for (size_t k = currentRoom; k < roomCount - currentRoom; k++)
                    {
                        if(k == roomCount - 1){
                            break;
                        }
                        Rooms[currentRoom] = Rooms[currentRoom + 1];
                    }
                    
					Rooms[roomCount] = emptyRoom;
					roomCount--;
				}

            char *final = &notify[0];//returns the string that should be printed.
            return final;
}

void exitProgram(int index){//exit program function.
    int isFound = 0;
    			for (size_t i = 0; i < roomCount; i++)//bütün odaları dolaş
				{
				
					for (size_t j = 0; j < Rooms[i].roomClientCount; j++)//hangi odada olduğunu bul,
					{						
						if(strcmp((Rooms[i].clients[j].structName), Client[index].structName) == 0){
							Rooms[i].clients[j] = empty;// bütün elemanları çıkan elemanın yerine kadar bir kaydır
                			for (size_t k = j; k < Rooms[i].roomClientCount-j; k++)
                			{
                  					  if(k == Rooms[i].roomClientCount - 1){
                       					 break;
                    				}
                    			Rooms[i].clients[k] = Rooms[i].clients[k+1];
                			}
							Rooms[i].clients[Rooms[i].roomClientCount-1] = empty;//son elemanı empty yapıp countu bir azalt, ve isInRoom booleanını sıfıra eşitle.
							Rooms[i].roomClientCount--;
							Client[index].isInRoom = 0;

							if((Rooms[i].roomClientCount) == 0){
                    			Rooms[i] = emptyRoom;
                    			for (size_t t = i; t < roomCount - i; t++)
                    			{
                        			if(t == roomCount - 1){
                            			break;
                        			}
                        			Rooms[i] = Rooms[i + 1];
                    			}
                    
							Rooms[roomCount] = emptyRoom;
							roomCount--;
						}
							isFound = 1;
							break;
						}					
					}
					if(isFound == 1){
						break;
					}
				}

}

char *doEnter(int currentRoom, int index, int clientSocket){//enter function
    			char notify[300];
				strcat(notify, "User ");
				strcat(notify, Client[index].structName);
				strcat(notify, " has joined Room ");
				strcat(notify, Rooms[currentRoom].roomName);
				strcat(notify, "\0");
				Rooms[currentRoom].clients[(Rooms[currentRoom].roomClientCount)] = Client[index];//hangi odaya katılıyorsa onun client arrayine şuanki client(yani bu Client[index])'ı atıyoruz.
				Client[index].isInRoom = 1;			
				Rooms[currentRoom].roomClientCount++;
                
                char *final = &notify[0];           
                return final;

}

void * doNetworking(void * ClientDetail){//function that works all time 

	struct client* clientDetail = (struct client*) ClientDetail;//client initilization.
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
    char nickname[15];
	int read = recv(clientSocket,nickname,15,0);
	nickname[read] = '\0';
    strcpy(clientDetail -> structName, nickname);//get the nickname and copy it to client's nickname

	printf("Client %s has connected to the server.\n", nickname);
   
    
    
	while(1){// infinite loop.
        
		char password[10];
		char data[1024];
		int read = recv(clientSocket,data,1024,0);//get the command. Such as -create, -list, etc.
		data[read] = '\0';

		char output[1024];

		if(strcmp(data,"-list") == 0){//when command is list;

			char x[5];
			int count;
			char notify[1024];
			char message[15];
			strcpy(message, "Rooms: \n");
			strcat(notify, message);
		
			if((Client[index].isInRoom) != 1){//If its not in a room it can work
                strcpy(notify, printList());//Call printlist function.						
			    send(Client[index].sockID,notify,1024,0);
			}
			else{//If this person is in a room it will warn him that it can not use this command.
				char error[100];
				strcpy(error, "You must be in lobby to use command '-list' ");
				send(Client[index].sockID,error,100,0);
			}
			


		}
		if(strcmp(data,"-msg") == 0){//when the command is -msg

			if((Client[index].isInRoom) == 1){//message only works when person is in a room.

				int curRoomNum = 0;int isFound = 0;
				read = recv(clientSocket,data,1024,0);
				data[read] = '\0';
            	char prompt[15];
            	char final_message[1024];
            	strcpy(prompt, ">");
            	strcat(final_message, nickname);
            	strcat(final_message, prompt);
            	strcat(final_message, data);
				strcat(final_message, "\0");

                curRoomNum = findRoom(index);//finds the current room that client is in.
				
            	for(int i = 0 ; i < Rooms[curRoomNum].roomClientCount ; i++){//send this message only to persons in that spesific room.
			    	send(Rooms[curRoomNum].clients[i].sockID,final_message,1024,0);	
            	}		
			}
			else{// if person is in lobby it should not work.
				char error[100];
				strcpy(error, "You must be in a room to chat.");
				send(Client[index].sockID,error,100,0);
			}
			

		}
		if(strcmp(data,"-enter") == 0){//when command is enter

			char notify[300];
			int isFound = 0;
			int currentRoom;
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
			char tmpRoomName[20];
			strcpy(tmpRoomName, data);
			if((Client[index].isInRoom) != 1){

            currentRoom = findRoomByName(tmpRoomName);//find which room is he/she wants to enter.

			
			if((currentRoom != -1) && ((Rooms[currentRoom].isPrivate) == 0)){// if the room is public
				
                send(Client[index].sockID, "Public\0", 8, 0);
                strcpy(notify, doEnter(currentRoom, index, clientSocket));//call doEnter function and send notification to clients.
				send(Client[index].sockID,"You entered the room.",300,0);
				for(int i = 0 ; i < Rooms[currentRoom].roomClientCount ; i ++){
			    	send(Rooms[currentRoom].clients[i].sockID,notify,300,0);	
            	}	
			}
			else if((currentRoom != -1) && ((Rooms[currentRoom].isPrivate) == 1)){//if room is private
                    send(Client[index].sockID, "Private\0", 9, 0);
					char password[10];
					read = recv(clientSocket,password,10,0);//get password
					password[read] = '\0';
				if(strcmp(Rooms[currentRoom].password, password) == 0){//check if password is correct
                    strcpy(notify, doEnter(currentRoom, index, clientSocket));
				    for(int i = 0 ; i < Rooms[currentRoom].roomClientCount ; i ++){
			    	    send(Rooms[currentRoom].clients[i].sockID,notify,300,0);	
            	    }
				}
				else{//if password is wrong
					char error[100];
					strcpy(error, "Wrong password! \n\0");
					send(Client[index].sockID,error,100,0);
				}

			}
				else{//if there's no such a room that person wants to enter
					char error[50];
					strcpy(error, "Room not found!\n\0");
					send(Client[index].sockID,error,50,0);
				}
			}
			else{//if person is already in a room he/she can not use -enter command.
				char error[50];
				strcpy(error, "You can not use '-enter' command in a room.\n\0");
				send(Client[index].sockID,error,50,0);
			}
			
			

		}
		if(strcmp(data,"-create") == 0){//when command is create:
			
			if((Client[index].isInRoom) != 1){//if it's not currently in a room
                char notify[100];			
				strcpy(notify ,createRoom(index, clientSocket));//call createRoom function.
                send(Client[index].sockID,notify,100,0);
			}
			else{//if person is in a room he/she can not use this command.
				char error[50];
				strcpy(error, "You can not use '-create' command in a room.\n\0");
				send(Client[index].sockID,error,50,0);
			}
			
			
		}
		if(strcmp(data,"-pcreate") == 0){//when command is -pcreate
			
			
			if((Client[index].isInRoom) != 1){//if person is not in a room

                char notify[100];				
				strcpy(notify ,createPRoom(index, clientSocket));
                read = recv(clientSocket,password,10,0);//receive the password that client enters.
				password[read] = '\0';
				strcpy(Rooms[roomCount-1].password, password);
				send(Client[index].sockID,notify,100,0);
			}
			else{//if person is in a room he/she can not use this command.

				char error[50];
				strcpy(error, "You can not use '-pcreate' command in a room.\n\0");
				send(Client[index].sockID,error,50,0);
			}
			
		}
		if(strcmp(data,"-quit") == 0){
			
			if((Client[index].isInRoom) == 1){//if client is in a room 
				int isFound = 0;
				int currentRoom;
				read = recv(clientSocket,data,1024,0);
				data[read] = '\0';
				char tmpRoomName[20];
				strcpy(tmpRoomName, data);

				currentRoom = findRoomByName(tmpRoomName);
				
				if(currentRoom != -1){//if we found he/she is in a room
					for (size_t i = 0; i < Rooms[currentRoom].roomClientCount; i++)
					{
						if( strcmp(Rooms[currentRoom].clients[i].structName, Client[index].structName) == 0){
							isFound = 1;
						}
					}
					if(isFound == 1){//if everything is right, just call the quit function and send notification to those in that room.
                		char notify[100];
                		strcpy(notify,quit(index, currentRoom));
						send(Client[index].sockID,notify,100,0);
						for(int i = 0 ; i < Rooms[currentRoom].roomClientCount ; i ++){
			    			send(Rooms[currentRoom].clients[i].sockID,notify,100,0);	
            			}		
					}
					else{//If he/she is not in the room that they wanted to quit from.
						send(Client[index].sockID, "You are not in THIS room", 25, 0);
					}
				}
				else{//if there's no such room
					char notify[100];
					strcpy(notify, "Room not found!");
					send(Client[index].sockID,notify,100,0);
					}
			}
		else//if person is not in a room, he/she can not use this command.
		{
			char notify[100];
			strcpy(notify, "You can not quit the room because you are not in a room!");
			send(Client[index].sockID,notify,100,0);
		}
						
	}

		if(strcmp(data,"-whoami") == 0){//when who am i command comes, this part will work.
            char holder[100];
            strcpy(holder, whoami(nickname));//calls whoami function, and copies the value that function returns.
			send(Client[index].sockID,holder,100,0);//sends the string to client.	            	

		}

		if(strcmp(data,"-exit") == 0){//this is the code part for exit command.
			if((Client[index].isInRoom) == 1){// if it's in a room,
				exitProgram(index);//this part will work.
			} 
			//else, it will quit directly in client side of the code.
		}
	}

	return NULL;

}

int main(){
	//server creating part
	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NO);//giving port number that we decide in the beginning.
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

	//bind
	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;
	//listen
	if(listen(serverSocket,1024) == -1) return 0;

	printf("Server waiting for connections on port 3205 ...........\n");

	while(1){//infinite loop to make clients connect       
		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;
		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);
		clientCount ++; 
	}

	for(int i = 0 ; i < clientCount ; i ++)//join these threads.
		pthread_join(thread[i],NULL);

}