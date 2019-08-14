
/*********************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/13/2019
 * Class Information: Operating Systems I (CS_344_400)
 * Name: otp_enc.c
 * Description:
 * ********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues


/*********************************************************************************************
 * Method Name: process_file
 * input: filename of the file to be processed into buffer
 * output: pointer to the buffer
 * Description: This method processes the files indicated by the user. It also checks for 
 * 	bad characters.
 * ********************************************************************************************/

char * process_file(char filename[])
{
	FILE *fptr; // file variable
	char buffer[70000]; // buffer
	char * ptrbuffer; // pointer to buffer
	int charctr=0; // incrementor

	// make sure the buffer is empty	
	memset(buffer, '\0', sizeof(buffer));

	// Open file and throw error if needed
	fptr=fopen(filename, "r");
	if(fptr == NULL)
	{
		fprintf(stderr, "File does not exist or can not be opened.");
		exit(1);
	}
		

	// add the first character to the buffer
	buffer[charctr] = fgetc(fptr);
	while(buffer[charctr] != EOF)
	{

		// check for bad characters as it is added to the buffer
		if(buffer[charctr] < 'A' || buffer[charctr] > 'Z') 
		{
			if(buffer[charctr] != 32 && buffer[charctr] != 10)
			{
				fprintf(stderr, "Error bad characters\n");
				exit(1);
			}
		}

		charctr++;
		
		//add next character to buffer
		buffer[charctr] = fgetc(fptr);
	}

	//close file
	fclose(fptr);

	// create the pointer to buffer
	ptrbuffer = buffer;

	return ptrbuffer;	
}



int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
	char *	plaintext; // pointers to buffers of file contents
	char *	mykey;
    
	if (argc != 4) 
	{ 
		fprintf(stderr,"USAGE: %s <file> <file> <port#> \n", argv[0]); 
		exit(0); 
	} // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	strcpy(buffer, "otp_enc here!");
	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	// if the server sends back anything but "ok" send error
	if ((strcmp(buffer, "ok\0")) != 0)
	{
		fprintf(stderr, "Trying to connect to wrong location.\n");
		exit(2);
	}

	plaintext = process_file(argv[1]);
	mykey = process_file(argv[2]);

	// Check to make sure both files are the exact length
	if(sizeof(plaintext) != sizeof(mykey))
	{
		fprintf(stderr, "Files are not the same length");
		exit(1);
	}





	close(socketFD); // Close the socket
	return(0);
}
