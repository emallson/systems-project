#pragma once
#include <linux/linkage.h> 
#include <linux/kernel.h> 
#include <asm/uaccess.h>
#include "Globals.h"

// max buff size
#define MAX_BUF_SIZE 256
#define MAX_STORAGE_SIZE 20
#define NULL_ATTRIBUTE_LENGTH -2

/*
	This will get a variable based on what is passed in through VariableName. A length of a buffer is given to
	DefinitionLength. if DefinitionLength is greater than the max buffer size, a -1 is returned.

	If the VariableName is found within the list of saved variables, the VariableDefinition buffer is filled
	with DefinitionLength - 1 bytes and then null terminated at VariableDefinition[DefinitionLength]
	If the variable is not found, a -1 is returned
	*/

asmlinkage int sys_GetVariable(char __user *VariableName, char __user *VariableDefinition, int DefinitionLength)
{
	// of course, our temp buffer
	char TempBuffer[MAX_BUF_SIZE];
	char DefinitionBuffer[MAX_BUF_SIZE];

	if (DefinitionLength < 1 || DefinitionLength > MAX_BUF_SIZE)
	{
		// debug
		printk(KERN_EMERG "The DefinitionLength was either too low or too high. DefinitionLength value: %d \n", DefinitionLength);
		return (-1);
	}

	int VariableLength = strlen(VariableName);
	// print the incoming variable name for debugging purposes
	printk(KERN_EMERG "Starting up the GetVariable function. Params are VariableName: %s. \n", VariableName);

	// copy over from user space
	if (copy_from_user(TempBuffer, VariableName, VariableLength))
	{
		printk(KERN_EMERG "SaveVariable has failed: copy_from_user() (definition) error \n");
		return(-1);
	}
	
	// ALWAYS null terminate the stupid string.....another 2 hours of compiling because of this
	TempBuffer[VariableLength] = '\0';

	//debug
	printk(KERN_EMERG "The current TempBuffer: %s \n", TempBuffer);


	// debug stuff
	printk(KERN_EMERG "Variable count succesfull. The count is: %d \n", StoredVariables);

	// we now have the number of attributes stored
	// now to sift through the variable names and see if theyre equivalent
	int TempNum;
	for(TempNum = 0; TempNum < StoredVariables; TempNum++)
	{
		// need a temp buffer to store the stuff
		char CurrentStoredVariable[MAX_BUF_SIZE];
		strncpy(CurrentStoredVariable, VariableStorage[TempNum], VariableLength);

		// ALWAYS null terminate it
		CurrentStoredVariable[VariableLength] = '\0';

		//debug
		printk(KERN_EMERG "What was temporarily copied: %s \n", CurrentStoredVariable);

		// remember, we still have the value stored into TempBuffer
		if (!(strcmp(CurrentStoredVariable, TempBuffer)))
		{
			// if you come into this if statement, theyre the same variable. Load in the variables into
			// the appropriate buffers

			// we have a valid definition length, we now know how many bytes to copy. copy it back to USER SPACE		
			copy_to_user(VariableDefinition, VariableDefinitions[TempNum], DefinitionLength);

			// ALWAYS NULL TERMINATE....stupid bugs....
			VariableDefinition[DefinitionLength] = '\0';

			// debug
			printk(KERN_EMERG "Varable was copied to CurrentStoredVariable. The Value is: %s \n", CurrentStoredVariable);

			// if you get here, the lengths are correct and the buffers have been loaded
			printk(KERN_EMERG "The variable find was successful. The variable passed in: %s, The variable definition found: %s, The variable length: %d \n",
				TempBuffer, VariableDefinition, DefinitionLength);
			return(0);
		}
	}

	// if you make it here, the variable was not found. Return a -1
	return (-1);
}

