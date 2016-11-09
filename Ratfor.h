#pragma once
#include <assert.h>
#include <string.h>

#define L_NOT(a)   (!a)
#define XOR(a,b) (a ^ b)
#define AND(a,b) (a & b)
#define GETC( c, file) (c = getc(file))

#define EOS     '\0'
#define NEWLINE '\n'
#define BLANK   ' '
#define TAB     '\t'
#define BACKSPACE '\b'
#define NOSKIP  '+'
#define SKIP    '-'
#define YES     1
#define NO      0
#define MAXLINE 160	
#define MAXCHUNK 254   //could be 511, 1023 etc
#define THRESH 5
#define RCODE '\b'
#define NOT '!'
#define MAXARRAY 1024
#define MAXSET   1024
#define ESC '\@'  //escape char is @n is new line, can change if needed
//Forward declerations
int tabpos(int col, int *tabs);
void settabs(int *tabs);
int mod(int a, int b);
int max(int a, int b);
int addset(char c, char *set, int *j, int maxsize);

/*
 Chapter One
*/
void copy(FILE *inFile, FILE *outFile) {
	
	assert(inFile != NULL);
	assert(outFile != NULL);

	int c;
	
	while ((c = fgetc(inFile)) != EOF) {
		fputc(c, outFile);
	}
}

int charCount(FILE *inFile) {
	assert(inFile != NULL);

	int nc;
	nc = 0;

	while (fgetc(inFile) != EOF) {
		nc++;
	}

	return nc;
}

int lineCount(FILE *inFile) {
	assert(inFile != NULL);
	int c, lc;

	lc = 0;

	while ((c = fgetc(inFile)) != EOF) {
		if (c == NEWLINE) {
			lc++;
		}
	}
	return lc;
}

int wordCount(FILE *inFile) {
	assert(inFile != NULL);
	int wc;
	int c;
	int inword;

	wc = 0;
	inword = NO;
	while ((c = fgetc(inFile)) != EOF) {
		if ((c == BLANK) | (c == NEWLINE) | (c == TAB)) {
			inword = NO;
		}
		else if( inword == NO)
		{
			inword = YES;
			wc++;
		}
	}

	return wc;
}

void printTabs(int *tabs) {
	for (int i = 0; i < MAXLINE; i++) {
		if (tabs[i] == YES) {
			printf("1");
		}
		else if (tabs[i] == NO) {
			printf("0");
		}
		printf(".");
	}
	printf("\n");
}

void deTab(FILE *inFile, FILE *outFile) {
	int c, col;
	int tabs[MAXLINE];

	settabs( tabs );
	
	col = 1;

	while ((c = fgetc(inFile)) != EOF) {
		if (c == TAB) {
			do {
				fputc(BLANK, outFile);
				//putc(BLANK,stdout);
				col++;
			} while ( tabpos(col, tabs) == YES);	
		}
		else if (c == NEWLINE) {
			fputc(NEWLINE, outFile);
			//putc(NEWLINE, stdout);
			col = 1;
		}
		else {
			fputc(c, outFile);
			//putc(c, stdout);
			col++;
		}
	}
}

int tabpos(int col, int *tabs) {
	//assert(col > 0);
	if (col > MAXLINE ) {
		return (int)YES;
	}

	return tabs[col];
}

void settabs(int *tabs) {

	for (int i = 0; i <= MAXLINE - 1; i++) {
		if (mod(i , 8) == 1) {
			tabs[i] = (int)YES;
		}
		else {
			tabs[i] = (int)NO;
		}
	}
}

int mod(int a, int b) {
	return a % b;
}

/*
Chapter Two
*/

void entab( FILE *inFile, FILE *outFile){
	assert(inFile != NULL);
	assert(outFile != NULL);

	int c, col, newcol, tabs[MAXLINE];

	settabs(tabs);
	//printTabs(tabs);
	c = 1;
	col = 1;
	while (1) {
		newcol = col;
		while ((c = fgetc(inFile)) == BLANK) { //collect blanks
			newcol++;
			if (tabpos(newcol, tabs) == YES) {
				fputc(TAB, outFile);
				col = newcol;
			}
		}
		for (; col < newcol; col++) {
			fputc(BLANK, outFile);
		}
		
		if (c == EOF) {
			//break;
			return;
		}
		else {
			fputc(c, outFile);
		}

		if (c == NEWLINE) {
			col = 1;
		}
		else {
			col++;
		}
	}
	printf("entab finished\n");
}

//redundant as not using a real tty, here for completeness
void overstrike(FILE *inFile, FILE *outFile) {
	int c, col, newcol;

	col = 1;
	while (1) {
		newcol = col;
		while ((c = fgetc(inFile)) == BACKSPACE) {
			newcol = max(newcol - 1, 1);
		}
		if (newcol < col) { //start overstrike line
			fputc(NEWLINE, outFile);
			fputc(NOSKIP, outFile);
			for (col = 1; col < newcol; col++) {
				fputc(BLANK, outFile);
			}
		}
		else if ((col == 1 )& (c != EOF)) {
			fputc(SKIP, outFile);
		}
		if (c == EOF){
			return;
		}
		if (c == NEWLINE) {
			col = 1;
		}
		else {
			col++;
		}
	}
}

int max(int a, int b) {
	if (a > b) return a;
	return b;
}

void putbuf(int *buff, int nsave, FILE *f) {
	int i;
	if (nsave > 0) {
		fputc(nsave, f);
		for (i = 0; i < nsave; i++) {
			fputc(buff[i], f);
		}
	}
}

void compress(FILE *inFile, FILE* outFile) {
	int c, lastc;
	int buff[MAXCHUNK];
	int nrep, nsave;

	nsave = 0;
	for (lastc = getc(inFile); lastc != EOF; lastc = c) {
		for (nrep = 1; (c = getc(inFile)) == lastc; nrep++) {
			if (nrep > MAXCHUNK) break;
		}
		if (nrep < THRESH) {
			for (; nrep > 0; nrep = nrep - 1) {
				
				buff[nsave] = lastc;
				nsave++;

				if (nsave > MAXCHUNK - 1) {
					putbuf(buff, nsave , outFile);
					nsave = 0;
				}
			}
		}
		else {
			putbuf(buff, nsave, outFile);
			nsave = 0;
			fputc(RCODE, outFile);
			fputc(lastc, outFile);
			fputc(nrep, outFile);
		}
	}
	putbuf(buff, nsave, outFile);
}

void expand(FILE *infile, FILE *outFile) {
	int c, code;

	while ((code = getc(infile)) != EOF) {
		if (code == RCODE) { //expand repition
			if ((c = getc(infile)) == EOF) {
				break;
			}
			if ((code = getc(infile)) == EOF) {
				break;
			}
			for (; code > 0; code = code - 1) {
				fputc(c, outFile);
			}
		}
		else {
			for (; code > 0; code = code - 1) {
				if ((c = getc(infile)) == EOF) {
					break;
				}
				fputc(c, outFile);
			}
			if (c == EOF) {
				break;
			}
		}
	}
}


void crypt(FILE *inFile, FILE *outFile, char *key) {
	int c;

	for (int i = 0; GETC(c, inFile) != EOF; i = mod(i, strlen(key) + 1)) {
		fputc( XOR( c, key[i]), outFile);
	} 
}

//Not quite the book implementation, this in is sure to return iff the string is null terminated
int index(char c, char *s) {
	char *i = s;

	while ((*i != c)) {
		if (*i == NULL) {
			return -1;
		}
		i++;
	}
	return  i - s;
}

int xindex(char c, char *s, int allbut, int lastto) {
	if (c == EOF) {
		return 0;
	} 
	else if (allbut == NO) {
		return index(c, s);
	}
	else if (index(c, s) > 0) {
		return 0;
	}
	return lastto + 1;
}

//Since c already has a length of a string function
#define length(s) strlen(s)

void filset(char delim, char *s, int i, char *set, int j, int size ) {
	char digits[] = "01234567890";
	char lowalf[] = "abcdefghijklmnopqrstuvwxyz";
	char upalf[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

}

int makset(char *s, int k, char *set, int size){
	int i, j;
	i = k;
	j = 1;
	filset(EOS, s, i, set, j, size);
	return addset(EOS, set, &j, size);
}

//Can you see how this will get messy!
int addset(char c, char *set, int *j, int maxsize) {
	if (*j > maxsize) {
		return NO;
	}
	else {
		set[*j] = c;
		*j = *j + 1;
	}
	return YES;
}
/*
 translit from to, returns *char
 Translit gets a little bit complicated
 We can do this via file streams or via char strings
*/
char *translit( char *s) {

	char *ret = NULL;
	char* from = NULL;

	/*
	 if char *s = "!A-Z a-z";
	 there are TWO arguments from getarg(n, array, MAXARRAY)
	*/
	//char s[] = "!w-z 0-9";

	int allbut;
	puts(s);
	char *token = strtok(s, " ");
	
	if (token[0] == NOT) {
		allbut = YES;
		if ( makset(token + 1, 2, from, MAXSET) == NO) {
			puts("from: to large");
			exit(EXIT_FAILURE);
		}
	}
	else {
		allbut = NO;
		puts(token+1);
	}

	token = strtok(NULL, " ");
	puts(token);

	return ret;
}

