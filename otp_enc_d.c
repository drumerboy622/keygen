
/*********************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/13/2019
 * Class Information: Operating Systems I (CS_344_400)
 * Name: otp_enc_d.c
 * Description: This file is for porject 4. It serves as the server side of the 2 way connection
 *   and is responsible for encrypting a message. It will take the message and key from the
 *   client and return the encryption. 
 * ********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { fprintf(stderr, "%s\n", msg); exit(0); } // Error function used for reporting issues



/*********************************************************************************************
 * Method: ciphertext
 * input: message and key
 * output: ciphertexted code
 * Description: This function will calculate the ciphertext
 * ********************************************************************************************/
char * ciphertext(char message[], char key[])
{
	char * result; // ciphertext code
	int charctr=0;

	//allocate memory
	result= (char *)malloc(70000 * sizeof(char));
	if(result == NULL)
	{
		error("Memory not allocated in server\n");
		exit(1);
	}

	//ciphertext according to the instructions 
	while(message[charctr] != '\0')
	{
		//replace the space with the [
		if(message[charctr] == 32)
			message[charctr] = 91;
		if(key[charctr] == 32)
			key[charctr] = 91;

		// message + key
		result[charctr] = message[charctr] + key[charctr] - 65;

		// meesage + key (mod 27)
		if (result[charctr] > 91)
			result[charctr] = result[charctr] - 27;

		if (result[charctr] == 91)
			result[charctr] = 32;

		charctr++;
	}

	return result;
}




int main(int argc, char * argv[])
{
	//Variables
	pid_t	spawnpid = 1; //pid variable
	int 	listenSocketFD, 
		establishedConnectionFD, 
		portNumber, 
		ptlength,
		charsRead;
	socklen_t 	sizeOfClientInfo;
	char 	buffer[70000];
	char	key[70000];
	char *	result;
	char * 	point_to_buf; // track the last byte of the last file sent
	
	struct sockaddr_in 	serverAddress, 
				clientAddress;

	// There should be 2 arguments when running this program
	if(argc != 2)
		error("The syntax for otp_enc_d is 'otp_enc_d listening_port'\n");

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
		error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");


	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while(1)
	{


		// Accept a connection, blocking if one is not available until one connects 
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) 
			error("ERROR on accept");

		// Fork off here
		if ((spawnpid = fork()) == -1)
		{
			error("fork() error");
		}
		else if (spawnpid == 0)
		{		


			// Get the message from the client and determine if it is otp_enc 
			memset(buffer, '\0', 70000);
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) 
				error("ERROR reading from socket");
	
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
				error("ERROR writing to socket");

			sleep(2);	
			// receieve the message from the client	
			memset(buffer, '\0', 70000);
			charsRead = recv(establishedConnectionFD, buffer, 70000, 0); // Read the client's message from the socket
			if (charsRead < 0) 
				error("ERROR writing to socket");



			// Send a quick ok to block the server from sending the 2 files together
			charsRead = send(establishedConnectionFD, "ok", 3, 0); // Send success back

			sleep(2);
			memset(key, '\0', 70000);
			// recieve the key from the client
			charsRead = recv(establishedConnectionFD, key, 70000, 0); // Read the client's message from the socket
			if (charsRead < 0) 
				error("ERROR writing to socket");

			result = ciphertext(buffer, key);

			// In order to send full file, make sure to keep track of the number of bytes sent
			ptlength = strlen(result);
			point_to_buf = result;
			while(ptlength > 0)
			{
				// Send the code
				charsRead = send(establishedConnectionFD, point_to_buf, 70000, 0);
				if(charsRead < 0) error("CLIENT: ERROR writing to socket\n");
				
				ptlength = ptlength - charsRead;
                		point_to_buf = point_to_buf + charsRead;
			}

			free(result);



			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			exit(0);
		}
	
	}	
}

