
/*********************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/13/2019
 * Class Information: Operating Systems I (CS_344_400)
 * Name: otp_enc_d.c
 * Description:
 * ********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char * argv[])
{
	//Variables
	pid_t	spawnpid = 1; //pid variable
	int 	listenSocketFD, 
		establishedConnectionFD, 
		portNumber, 
		charsRead;
	socklen_t 	sizeOfClientInfo;
	char 	buffer[69336];
	struct sockaddr_in 	serverAddress, 
				clientAddress;

	// There should be 2 arguments when running this program
	if(argc != 2)
		fprintf(stderr, "The syntax for otp_enc_d is 'otp_enc_d listening_port'\n");

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 
	// Clear out theaddress struct
	
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) 
		fprintf(stderr, "ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		fprintf(stderr, "ERROR on binding");


	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while(1)
	{


		// Accept a connection, blocking if one is not available until one connects 
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) 
			fprintf(stderr, "ERROR on accept");

		// Fork off here
		if ((spawnpid = fork()) == -1)
		{
			fprintf(stderr, "fork() error");
		}
		else if (spawnpid == 0)
		{		


			// Get the message from the client and display it
			memset(buffer, '\0', 256);
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) 
				fprintf(stderr, "ERROR reading from socket");
	
			if (strcmp(buffer, "otp_enc here!\0") == 0)
			{
				//Send a Success message back to the client
				charsRead = send(establishedConnectionFD, "ok", 3, 0); // Send success back
			}
			else
			{
				//Send a a no access message back to the client
				charsRead = send(establishedConnectionFD, "no", 3, 0); // Send success back
			}
		
			if (charsRead < 0) 
				fprintf(stderr, "ERROR writing to socket");
	


			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			exit(0);
		}
	
	}	
}

