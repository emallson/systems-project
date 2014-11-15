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
This function will find the next variable in the list based on the PreviousName. If PreviousName is found, a check to see if we are at the
end of the list or not is done. If we are, notify the user we're at the end of the list. If we arent at the end of the list, return the
next variable name and definition back to the user.

Returns -2 if theres an error, -1 if end of list is found, 0 if variable is found.

*/
asmlinkage int sys_NextVariable(char __user *PreviousName, char __user *Variable, int VariableLength, char __user *VariableDefinition, int DefinitionLength)
{
	// debug
	printk(KERN_EMERG "Starting up the NextVariable function. Params are PreviousName: %s. \n", PreviousName);

	// get the size of the passed in buffer to make sure its not longer than our declared buffer
	int PreviousNameSize = strlen(PreviousName);

	// error checking
	if (PreviousNameSize < 0 || PreviousNameSize > MAX_BUF_SIZE)
	{
		// debug
		printk(KERN_EMERG "Length of the passed in buffer, PreviousName is too big or too small. The buffer size: %d", PreviousNameSize);
		return (-2);
	}

	// error checking
	if (VariableLength < 0 || VariableLength > MAX_BUF_SIZE)
	{
		// debug
		printk(KERN_EMERG "Length of the passed in buffer, VariableLength,  is too big or too small. The buffer size: %d", VariableLength);
		return (-2);
	}

	// error checking
	if (DefinitionLength < 0 || DefinitionLength > MAX_BUF_SIZE)
	{
		// debug
		printk(KERN_EMERG "Length of the passed in buffer, DefinitionLength,  is too big or too small. The buffer size: %d", DefinitionLength);
		return (-2);
	}

	// since we have an adequate buffer size across the board. copy the stuff over

	// create a buffer to store PreviousName in
	char PreviousBuffer[MAX_BUF_SIZE];

	// time to copy from user space to kernel space
	copy_from_user(PreviousBuffer, PreviousName, PreviousNameSize);

	// Null terminate. Holy crap i missed alot of these...
	PreviousBuffer[PreviousNameSize] = '\0';

	//debug
	printk(KERN_EMERG "The variable was saved into kernel space. What was saved: %s \n", PreviousBuffer);


	// we now have the previous variable stored in a buffer. now its time to search through the list and see where we stand.


	// first check to see if there are even any variables in here
	if (StoredVariables == 0)
	{
		// no variables are stored. Return a -1 to indicate the end of the list
		printk(KERN_EMERG "The variable list is empty. Returning -1 to indicate end of list");
		return (-1);
	}

	// so there is at least ONE variable in the list. return it

	// if they are the SAME, return the first in the definition and saved variables
	if (!(strcmp(PreviousBuffer, "")))
	{
		char TempVariableStorage[MAX_BUF_SIZE];
		char TempDefinitionStorage[MAX_BUF_SIZE];

		//debug
		printk(KERN_EMERG "The previous buffer was empty, indicating we want to start at the beggining. \n");

		// now this should be quick and simple, since we know we have valid lengths, just copy them into the buffers
		// plain and simple

		// do the copy!!
		strncpy(TempVariableStorage, VariableStorage[0], VariableLength);
		TempVariableStorage[VariableLength - 1] = '\0';
		copy_to_user(Variable, TempVariableStorage, VariableLength);

		strncpy(TempDefinitionStorage, VariableDefinitions[0], DefinitionLength);
		TempDefinitionStorage[DefinitionLength - 1] = '\0';
		copy_to_user(VariableDefinition, TempDefinitionStorage, DefinitionLength);

		return (0);
	}

	// if we make it here, the previous buffer isnt empty and we need to find out where the we need to go

	//debug
	printk(KERN_EMERG "The size of the variable lis has been retrieved. The size is: %d \n", StoredVariables);

	// we know how many variables there are, time to do a search
	int LCV;
	for (LCV = 0; LCV < StoredVariables; LCV++)
	{
		//debug
		printk(KERN_EMERG "The previous buffer was not empty. Entering loop... \n");

		// compare the string to the current place (LCV) in the loop.
		// if it is the SAME. We found the spot we were at. If not, carry on
		if (!(strcmp(PreviousBuffer, VariableStorage[LCV])))
		{
			char TempVariableStorage[MAX_BUF_SIZE];
			char TempDefinitionStorage[MAX_BUF_SIZE];

			// now this should be quick and simple, since we know we have valid lengths, just copy them into the buffers
			// plain and simple

			//debug
			printk(KERN_EMERG "Previous varable found! Returning next variable at location: %d unless it is the end of the list. \n", LCV + 1);

			// do what the previous debug says...check to see if there is even another variable in the list. If there isnt, return -1
			if (VariableStorageLength[LCV + 1] == NULL_ATTRIBUTE_LENGTH || LCV == 19)
			{
				//debug
				printk(KERN_EMERG "End of list found. The list is %d long with it's final list value being: %s \n", LCV, VariableStorage[LCV]);

				return (-1);
			}

			// we are not at the end of the list, do the copy
			// remember....LCV is at the PREVIOUS variable
			strncpy(TempVariableStorage, VariableStorage[LCV + 1], VariableLength);
			TempVariableStorage[VariableLength - 1] = '\0';
			copy_to_user(Variable, TempVariableStorage, VariableLength);

			strncpy(TempDefinitionStorage, VariableDefinitions[LCV + 1], DefinitionLength);
			TempDefinitionStorage[DefinitionLength - 1] = '\0';
			copy_to_user(VariableDefinition, TempDefinitionStorage, DefinitionLength);

			return (0);
		}
	}

	// if you make it here, you screwed up somehow....

	//debug
	printk(KERN_EMERG "Somewhere, something went wrong. The variables passed in are; PreviousVariable: %s, VariableLength: %d, DefinitionLength: %d \n",
		PreviousName, VariableLength, DefinitionLength);

	// careful, -2 is a null attribute value but also indicates failure in this case
	return (-2);
}
