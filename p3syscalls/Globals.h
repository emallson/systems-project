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

// max buff size is given in the hello world example
extern int MAX_BUF_SIZE;
// the max storage size declared by the specifications
extern int  MAX_STORAGE_SIZE;
extern int NULL_ATTRIBUTE_LENGTH;

extern char VariableStorage[][256];
extern int StoredVariables;

// the variable storage length is the same length as the first array in variable storage
// the position of the number in VariableStorageLength will denote the length of the string in
// the corresponding array position in VariableStorage.
// !!! NOTE! Theyre declared -2 (equal to the null size value) to indicate nothing has been stored there yet
extern int VariableStorageLength[];

// Variable definitions will have the same layout as the VariableStorage above
// The name should basically speaks for itself....but if it doesnt, this stores
// the definitions of the corresponding variables. Again, it's all based on location.
// location 0 in VariableStorage corresponds to location 0 in VariableDefinitions abd ub VariableStorageLength
extern char VariableDefinitions[][256];

// the length of the variable definitions
extern int  VariableDefinitionsLength[];

