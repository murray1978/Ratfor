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
#define ESCAPE 0x1B //esc \e in gcc?
#define LETN 'n'
#define LETT 't'

#define YES     1
#define NO      0
#define MAXLINE 160	   //80 char lines gave issues
#define MAXCHUNK 254   //could be 511, 1023 etc
#define THRESH 5
#define RCODE '\b'
#define NOT '!'
#define MAXARRAY 1024
#define MAXSET   1024
#define MAXARR   1024

#define DASH '-'

//Forward declerations
int tabpos(int col, int *tabs);
void settabs(int *tabs);
int mod(int a, int b);
int max(int a, int b);
//int addset(char c, char set[], int *j, int maxsize);

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
		else if (inword == NO)
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

	settabs(tabs);

	col = 1;

	while ((c = fgetc(inFile)) != EOF) {
		if (c == TAB) {
			do {
				fputc(BLANK, outFile);
				//putc(BLANK,stdout);
				col++;
			} while (tabpos(col, tabs) == YES);
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
	if (col > MAXLINE) {
		return (int)YES;
	}

	return tabs[col];
}

void settabs(int *tabs) {

	for (int i = 0; i <= MAXLINE - 1; i++) {
		if (mod(i, 8) == 1) {
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

void entab(FILE *inFile, FILE *outFile) {
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
		else if ((col == 1)& (c != EOF)) {
			fputc(SKIP, outFile);
		}
		if (c == EOF) {
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
					putbuf(buff, nsave, outFile);
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
		fputc(XOR(c, key[i]), outFile);
	}
}

//Not quite the book implementation, this in is sure to return iff the string is null terminated
int index(char c, char *s) {
	char *i = s;

	while ((*i != c)) {
		if (*i == NULL) {
			return EOF;
		}
		i++;
	}
	return  i - s;
}

int xindex(char s[], char c, int allbut, int lastto) {
	if (c == EOF) {
		return 0;
	}
	else if (allbut == NO) {
		return index(c, s);
	}
	else if (index(c, s) > 0) {
		return 0;
	}
	return lastto;
}

//Since c already has a length of a string function
#define length(s) strlen(s)

char esc(char s[], int i) {
	if (s[i] != ESCAPE) {
		return s[i];
	}
	else if (s[i + 1] == EOS) {
		return ESCAPE;
	}
	else {
		i = i + 1;
		if (s[i] == LETN) {
			return NEWLINE;
		}
		else if (s[i] == LETT) {
			return TAB;
		}
		else {
			return s[i];
		}
	}
}

//Can you see how this will get messy!
int addset(char c, char set[], int *j, int maxsize) {
	if (*j >= maxsize) {
		return NO;
	}
	else {
		set[*j] = c;
		*j = *j + 1;
	}
	return YES;
}

//modifies j, i
void dodash(char valid[], char s[], int i, char set[], int *j, int size) {
	//Hope this is contents of, not value of
	i = i + 1;
	j = j + 1;
	int junk;

	int limit = index(esc(s, i), valid);
	for (int k = index(set[*j], valid); k <= limit; k++) {
		junk = addset(valid[k], set, j, size);
	}

}


void filset(char delim, char s[], int i, char set[], int *j, int size) {
	int junk;

	char digits[] = "01234567890";
	char lowalf[] = "abcdefghijklmnopqrstuvwxyz";
	char upalf[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (; (s[i] != delim) & (s[i] != EOS); i++) {
		if (s[i] == ESCAPE) {
			junk = addset(esc(s, i), set, j, size);
		}
		else if (s[i] != DASH) {
			junk = addset(s[i], set, j, size);
		}
		else if (*j <= 1 | s[i + 1] == EOS) { //Lterall -
			junk = addset(DASH, set, j, size);
		}
		else if (index(s[*j - 1], digits) > 0) {
			dodash(digits, s, i, set, j, size);
		}
		else if (index(s[*j - 1], lowalf) > 0) {
			dodash(lowalf, s, i, set, j, size);
		}
		else if (index(s[*j - 1], upalf) > 0) {
			dodash(upalf, s, i, set, j, size);
		}
		else {
			junk = addset(DASH, set, j, size);
		}
	}
	set[*j] = '\0'; //getting a bit paranoid
}

void swap(int *px, int *py) {
	int temp;
	temp = *px;
	*px = *py;
	*py = temp;
}

int makset(char s[], int k, char set[], int size) {
	int i, j;
	i = k;
	j = 0; //1
	filset(EOS, s, i, set, &j, size);
	j++;
	return addset(EOS, set, &j, size); //adding the EOS
}


/*
 Something in our "lovely" code failed
*/
void error(char msg[]) {
	printf(msg);
	exit(EXIT_FAILURE);
}


/*
 translit from to, returns *char
 Translit gets a little bit complicated
 We can do this via file streams or via char strings
 <RATFOR>
	translit - map characters
	character getc
	character arg(MAXARR), c, from(MAXSET), to(MAXSET)
	integer getarg, length, markset, xindex
	integer allbut, collap, i, lastto

	if (getarg(1, arg, MAXARR) == EOF)
		call error("usage:translit from to")
	else if (arg(1) == NOT){
		allbut = yes
		if (markset(arg, 2, from, MAXSET) == NO )
			call error("From: too large")
	}
	else {
		allbut = NO
		if( markset(arg, 1, from, MAXSET) == NO )
			call error("From: to large")
	}

	if( getarg(2, arg, MAXARR) == EOF )
		to(1) = EOF
	else if( markset(arg, 1, to, MAXSET) == NO )
		call error("TO: too large")

	lastto = length(to)
	if( length(from) > lastto | allbut == YES )
		callop = YES
	else
		callop = NO

	repeat{
		i = xindex( from, getc(c), allbut, lastto)
		if( collap == YES & i >= lastto & lastto > 0 ){ #collapse
			call putc(to(lastto))
			repeat{
				i = xindex( from, getc(c), allbut, lastto)
			}until( i < lastto)
		}#end collapse
		if( c == EOF )
			break
		if( i > 0 & lastto > 0)
			call putc(to(i))
		else if( i == 0 )
			call putc(c)
	}
 </RATFOR>
*/
int translit(char argFrom[], char argTo[], char input[], char *output) {
	//translit - map characters
	//character getc
	//character arg(MAXARR), c, from(MAXSET), to(MAXSET)
	char from[MAXSET];
	char to[MAXSET];
	//integer getarg, length, markset, xindex
	int length;
	//integer allbut, collap, i, lastto
	int allbut, collap, i, lastto;

	//if (getarg(1, arg, MAXARR) == EOF)
	//	call error("usage:translit from to")
	//else if (arg(1) == NOT) {
	if (argFrom[0] == NOT) {
		allbut = YES;
		if (makset(argFrom, 1, from, MAXSET) == NO) { //"!A"
			error("From: too large");
		}
	}
	else {
		allbut = NO;
		if (makset(argFrom, 0, from, MAXSET) == NO) //"A"
			error("From: to large");
	}

	if (argTo == NULL) {
		to[0] = EOF;
	}
	else if (makset(argTo, 0, to, MAXSET) == NO)
		error("TO: too large");

	lastto = length(to);
	if (length(from) > lastto | allbut == YES)
		collap = YES;
	else
		collap = NO;

	while (1) {
		//i = xindex(from, getc(input), allbut, lastto);
		i = xindex(from, *input++, allbut, lastto);
		if (collap == YES & i >= lastto & lastto > 0) {
			//#collapse
			//putc(to[lastto], output);
			*output = to[lastto];
			output += 1;
			do {
				//i = xindex(from, getc(input), allbut, lastto);
				i = xindex(from, *input++, allbut, lastto);
			} while (i < lastto);
		}//#end collapse
		if (*input == EOF) //c
			break;
		if (i > 0) {
			//putc(to[i]);
			*output = to[i];
			output += 1;
		}
		else if (lastto > 0) {
			*output = to[i];
			output += 1;
		}
		else if (i == 0) {
			//putc(c);
			*output = *input;
			output += 1;
		}
	}
	output[MAXARR - 1] = '\0'; //truncate some data
	return YES;
}

