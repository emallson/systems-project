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
The layout of the variable storage will be given as follows

VariableStorage [V][w][b][s][l][t][-][-]...[19]
				[a][i][e][t][i][h][-][-]...[19]
				[r][l][-][o][k][i][-][-]...[19]
				[i][l][-][r][e][s][-][-]...[19]
				[a][-][-][e][-][-][-][-]...[19]
				[b][-][-][d][-][-][-][-]...[19]
				[l][-][-][-][-][-][-][-]...[19]
				[e][-][-][-][-][-][-][-]...[19]
				[s][-][-][-][-][-][-][-]...[19]
*/
char VariableStorage[MAX_STORAGE_SIZE][MAX_BUF_SIZE];
int StoredVariables;


// the variable storage length is the same length as the first array in variable storage
// the position of the number in VariableStorageLength will denote the length of the string in
// the corresponding array position in VariableStorage.
// !!! NOTE! Theyre declared -2 (equal to the null size value) to indicate nothing has been stored there yet
int VariableStorageLength[MAX_STORAGE_SIZE] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};

// Variable definitions will have the same layout as the VariableStorage above
// The name should basically speaks for itself....but if it doesnt, this stores
// the definitions of the corresponding variables. Again, it's all based on location.
// location 0 in VariableStorage corresponds to location 0 in VariableDefinitions abd ub VariableStorageLength
char VariableDefinitions[MAX_STORAGE_SIZE][MAX_BUF_SIZE];

// the length of the variable definitions
int VariableDefinitionsLength[MAX_STORAGE_SIZE] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2 -2};

/*
	This is the Save Variable function. It accepts a buffer of input from the user (__user *Buffer) and
	accepts the definition of the variable buffer (Definition). The length is tested against the MAX_BUF_SIZE. If the length
	is too big, a -1 is returned.
*/

asmlinkage int sys_SaveVariable(char __user *Variable, char __user *Definition) {

	char TempBuffer[MAX_BUF_SIZE]; // Temporary buffer for the user's input
	// if the length is 0 or greater than the max buffer size, write to the kernel log the length of the
	// buffer and return -1
	int Length = strlen(Variable);

	// some output to the kernel log so we can see what happens. Makes it easier for debugging....
	printk(KERN_EMERG "Starting up the SaveVariable function. The Length of the user's buffer is: %d \n", Length);
	

	if ((Length < 1) || (Length >= MAX_BUF_SIZE)) 
	{
		printk(KERN_EMERG "SaveVariable has failed: illegal length of buffer. (%d)! \n", Length);
		return(-1);
	}

	// copy buff from user space into a kernel buffer
	if (copy_from_user(TempBuffer, Variable, Length)) 
	{
		printk(KERN_EMERG "SaveVariable has failed: copy_from_user() error \n");
		return(-1);
	}
	
	// null terminate the string
	TempBuffer[Length] = '\0';

	//debug
	printk(KERN_EMERG "This was saved to temp buff: %s \n", TempBuffer);

	// log success in copying and valid data
	printk(KERN_EMERG "Valid data has been given\n");

	// now we need to go and store it. Valid data has been put in
	
	// sift through the size of the variable length array and find out what's been assigned
	int TempNum;
	TempNum = 0;

	// this is used to check and see if we are reassigning
	// a variable
	bool DuplicateItem;
	DuplicateItem = false;
	while (VariableStorageLength[TempNum] != NULL_ATTRIBUTE_LENGTH
		&& TempNum < 19 && !DuplicateItem)
	{
		// while we're in this loop, lets check for a "duplicate" and update it
                if (!(strcmp(VariableStorage[TempNum], TempBuffer)))
			// if we made it here, that means we need to reassign the spot. since the name's
			// the same, we only need to change the definition. Break so the number stops
			// incrementing
		{
			printk(KERN_EMERG "Duplicate name found. Updating current name. Duplicate: %s \n", TempBuffer);
			DuplicateItem = true;
		}

		if (!DuplicateItem)
			// havent found a duplicate or a free space. increment the counter
			TempNum++;
	}

	// check to see if the storage is absolutely full!
	if (TempNum == 19 && VariableStorageLength[TempNum] != NULL_ATTRIBUTE_LENGTH)
	{
                printk(KERN_EMERG "Failed to add %s. Global variable list is full....", Variable);
                return (-1);
        }


	// log where the data id going to be stored
	printk(KERN_EMERG "Variable %s will be stored in location %d.\n", TempBuffer, TempNum);

	// now that we have a valid location, put the user's value into the location
	VariableStorageLength[TempNum] = Length;

	// copy the string to the storage. The +1 is to include the null termination from the statement above
	// strncpy (destination, source, amount of bytes)
	strncpy(VariableStorage[TempNum], TempBuffer, Length + 1);

	// more debugging crap
	printk(KERN_EMERG "The following was stored into the kernel space buffer: %s \n", VariableStorage[TempNum]);

	
	//	Now to get the definition and the definition length.
	

	// find the length of the definition. Reuse Length
	Length = strlen(Definition);

	if ((Length < 1) || (Length >= MAX_BUF_SIZE)) 
	{
		printk(KERN_EMERG "SaveVariable has failed: illegal length of definitioin buffer. (%d)! \n", Length);
		return(-1);
	}

	//we have a valid length, now copy from user space to kernel space
	if (copy_from_user(TempBuffer, Definition, Length)) 
	{
		printk(KERN_EMERG "SaveVariable has failed: copy_from_user() (definition) error \n");
		return(-1);
	}

	// Always null terminate. Stupid bugs...
	TempBuffer[Length] = '\0';

	// now to debug by printing what was saved in the buffer
	printk(KERN_EMERG "The following definition was copied over from user space: \"%s\" \n", TempBuffer);

	// its now safe to copy the data to the storages space


	//we now have a valid length
	VariableDefinitionsLength[TempNum] = Length;

	// now to copy the data over
	strncpy(VariableDefinitions[TempNum], TempBuffer, Length + 1);

	printk(KERN_EMERG "The following values were saved. VariableStorage[%d]: %s, VariableStorageLength[%d]: %d, VariableDefinition[%d]: %s, VariableDefinitionLength[%d]: %d \n", 
		TempNum, VariableStorage[TempNum], TempNum, VariableStorageLength[TempNum], TempNum, VariableDefinitions[TempNum], TempNum, VariableDefinitionsLength[TempNum]);

	StoredVariables++;

	return(0);
}

