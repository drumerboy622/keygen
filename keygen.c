
/*********************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/13/2019
 * Class Information: Operating Systems I (CS_344_400)
 * Name: keygen.c
 * Description: As noted by the program 4 requirements, this program creates a key file of
 *    specified length. The characters generated are all 26 upper-case letters plus the 
 *    space character. This is a simple script.
 * ********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
	//Variables
	int 	specified_length, // holds length specified by user
		loop; // interator
	char	output_char; // holds character of output
	time_t	t; // used to initialize random number generator

	// There should be 2 arguments when running this program
	if(argc != 2)
	{
		fprintf(stderr, "The syntax for keygen is 'keygen keylength'\n");
		exit(1);
	}
	// converts char * into int
	sscanf(argv[1], "%d", &specified_length); 

	// uses the current time to aid the random generator to output different outputs 
	// each time
	srand((unsigned) time(&t));

	// randomly select one of the 27 allowed characters 
	for (loop=0; loop < specified_length; loop++)
	{
		output_char = ((rand() % 26)+64);

		// If the generator uses picks @, change it to a space
		if (output_char == '@')
			output_char = ' ';
		
		printf("%c", output_char);
	}

	printf("\n");

	return(0);
}
