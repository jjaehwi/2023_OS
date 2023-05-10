#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool FileExists(char *fileName)
{
	FILE *filePtr = fopen(fileName, "r");

	if (!filePtr){
	return false;
	}

	return true;
}

void Copy(char *out, char *in)
{
	int c;
	FILE *inPtr = fopen(in, "r");
	FILE *outPtr = fopen(out, "w");

	if(!inPtr){
		perror("Source file can't be opened: ");
		exit(1);
	}

	if(!outPtr){
		perror("Destination file can't be opened: ");
		exit(1);
	}

	while ((c = fgetc(inPtr)) != EOF){
		fputc(c, outPtr);
	}

	printf("\nSuccess\n");

	fclose(inPtr);
	fclose(outPtr);
}

void PromptUser(){
	char str[32], inStr[32], outStr[32];
	char *strPtr = str;
	char *inPtr = inStr;
	char *outPtr = outStr;

	while (strcmp(inStr, "")){
		printf("Enter name of source file: ");
		scanf("%s", strPtr);

		if(FileExists(strPtr)){
			strcpy(inStr, str);
			break;
		}
		else{
			printf("File does not exist. File can't be opened!\n");
			continue;
		}
	}

	while (strcmp(outStr, "")){
		printf("Enter name of destination file: ");
		scanf("%s", strPtr);

		if(!FileExists(strPtr)){
			strcpy(outStr, str);
			break;
		}
		else{
			printf("File already exists! Please enter a different name.\n");
			continue;
		}
	}

	Copy(outStr, inStr);
}

int main(){
	PromptUser();

	return EXIT_SUCCESS;
}
