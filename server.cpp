/*
    Live Server on port 8888
*/
#include <iostream>
#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;
int main(int argc , char *argv[])
{   char* x;
    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in server , client;
    int c;
    char *message;
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    puts("Bind done");
    //Listen to incoming connections
    listen(s , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while( 1 )
    {

        new_socket = accept(s , (struct sockaddr *)&client, &c);
        if (new_socket!=INVALID_SOCKET){
                closesocket(s);
                puts("Connection accepted");

                //Reply to the client
                message = "Hello Client , I have received your connection";
                send(new_socket , message , strlen(message) , 0);
                message = "Hello  , can you hear me";
                send(new_socket , message , strlen(message) , 0);
                while(true){
                    if(read(new_socket,x,1024)){
                        cout<<x;
                        cout<<"iam in";
                    }

                }
        }



    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return 1;
    }
    closesocket(s);
    WSACleanup();

    return 0;
}
}
