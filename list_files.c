#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <ncurses.h>

//functions
static const int asizeh = 256; /* max number of characters in a file - same as in struct dirent - do not change */
static int getdirfiles(char directory[], char database[][asizeh]);
static int getdirnum(char directory[]);
static int sortalpha(const void *p, const void *q);
static void program(char *array, bool parent, bool child); 

//string vars
char *string = NULL;
char *selprog;
size_t mem = 0, charsize = 0;
int ch = EOF;

//structs
typedef struct {
	int term_rows;
	int term_cols;
} WinProps;

typedef struct timespec timespec;


void main() {
	char pwd[] = "/usr/bin"; 

	const int avsize = getdirnum(pwd);

	char list[avsize][asizeh];
	getdirfiles(pwd, list);

	qsort(list, avsize, asizeh, sortalpha);

	initscr();
	noecho();
	cbreak();

	WinProps *wp;
	wp = malloc(sizeof(int));
	
	getmaxyx(stdscr, wp->term_rows, wp->term_cols);

	char uprompt[] = "Search for program: ";

	void printmatches() {
	int i;
	int selines = 1;
	for (i = 3; i < avsize; i++){	
	char test[asizeh];	

	if ((strncmp(list[i], string, strlen(string)) == 0) && (selines < wp->term_rows)) {	
		if(selines == 1) { 
			selprog = NULL;
			selprog = malloc(256 * sizeof(char));
			strcpy(selprog, list[i]);
		}
	
	
	strcpy(test ,list[i]);
	selines++;

	printw("%s\n", test);
			}
	else continue;
		}

	move(0, charsize + strlen(uprompt));
	}

	printw(uprompt);
	move(1,0);
	
	int i;
	int linetot = wp->term_rows + 2;

	for (i = 3; i < linetot; i++) {
		char temp[asizeh];
		strcpy(temp, list[i]);
		printw("%s\n", temp);
		}

	move(0, strlen(uprompt));

    while ((ch = getch()) != ('\n')) {
     switch (ch) {
	case 127: {
		if (charsize > 0) {
		mvdelch(0, strlen(uprompt) + charsize - 1);
		string[--charsize] = 0;
		move(1,0);
		clrtobot();

		printmatches();
			}
		break;
		}	


	default: { 
	printw("%c", ch);
	move(1,0);
	clrtobot();

        if (mem <= charsize) {
            mem += sizeof(char);
            string = realloc(string, mem);
        }

        string[charsize++] = ch;
	
	printmatches();
    	    	}
	   }
	}

	endwin();
	program(selprog, true, true);
	return;
} 

int sortalpha(const void *p, const void *q) {
	const char *cp = (const char *) p;
	const char *cq = (const char *) q;
	return strcmp(cp, cq);
}	

 
int getdirfiles(char directory[], char database[][asizeh])
{
    int asizev = 0;
    DIR *current = opendir(directory);
    struct dirent *dirstc;

    if (current == NULL) {
	printf("error");
	return 1;
	}
    
    while ((dirstc = readdir(current)) != NULL) {
	strcpy(database[asizev], dirstc->d_name);
	asizev++; 
	}
    
	closedir(current);
    return 0;
}

int getdirnum(char directory[]) {
	int i;
	struct dirent *dirstc;
	DIR *current = opendir(directory);

	if (current == NULL) printf("error");

	for(i = 0; (dirstc = readdir(current)) != NULL; i++);

	closedir(current);
	return i;
}

void program(char *array, bool parent, bool child) {	

	if (parent == true) {
	int parentpid = getpid();
	printf("parent process number is: %i\n", parentpid);
		}
	
	if (fork() == 0) {

	if (child == false) {	
	int out = open("/dev/null", O_WRONLY);
	dup2(out, 1);
	dup2(out, 2);
	close(out);
		}

	else {
	int childpid = getpid();
	printf("child process number is: %i\n", childpid);
		}

	execvp(array, NULL);
	exit(1);
	}

	else {
	timespec t;	
	t.tv_sec = 0;
	t.tv_nsec = 500000;

	nanosleep(&t, &t);
	printf("Successfully launched\n");	
	}
}
