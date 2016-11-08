#define _CRT_SECURE_NO_WARNINGS //Make fopen "unsecure"
#include <stdio.h>
#include <stdlib.h>
#include "Ratfor.h"


int main(int argc, char* argv[])
{
	FILE *testFile;
	testFile = fopen("test2.txt", "r");
	if (!testFile) {
		perror("File opening failed");
		return EXIT_FAILURE;
	}
	
	FILE *detabfile;
	detabfile = fopen("detab2.txt", "w+");
	if (!detabfile) {
		perror("File opening failed");
		return EXIT_FAILURE;
	}

	FILE* entabfile = fopen("entab2.txt", "w+");
	if (!entabfile) {
		perror("File opening failed");
		return EXIT_FAILURE;
	}

	FILE *compressFile = fopen("compress2.txt", "w+");
	if (!compressFile) {
		perror("file opening error");
		return EXIT_FAILURE;
	}
	
	FILE *unCompress = fopen("uncomp.txt", "w+");
	if (!compressFile) {
		perror("file opening error");
		return EXIT_FAILURE;
	}

	FILE *cryptFile = fopen("crypt.txt", "w+");
	if (!cryptFile) {
		perror("file opening error");
		return EXIT_FAILURE;
	}

	//entab(outFile, detabfile);
	deTab(testFile, detabfile);
	printf("detab finished\n");
	fflush(detabfile);

	rewind(detabfile);
	entab(detabfile, entabfile);
	
	rewind(detabfile);
	compress(detabfile, compressFile);

	rewind(compressFile);
	expand(compressFile, unCompress);

	rewind(testFile);
	crypt(testFile, cryptFile, "MonkeyMan");


	printf("Closing entabfile\n");
	fclose(entabfile);

	printf("closing detabfile\n");
	fclose(detabfile);

	printf("closing testFile\n");
	fclose(testFile);

	fclose(compressFile);
	fclose(unCompress);

	char testString[] = "MonkeyWentToIbiza";
	printf("%s W location is %i", testString, indexOf('W', testString));

	return EXIT_SUCCESS;
}