#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
 
#include <pulse/simple.h>
#include <pulse/error.h>
#include <stdlib.h>
#include <magic.h>

#include <stdbool.h>
#include <sys/stat.h>

typedef struct{
    char** data;
    int len;
}Array;

bool islink(char*);
char* readlnk(char*);
int IsNum(char);
int FullNum(char*);
char* gettype(char*);
Array split(char*, const char*);
int charpToInt(char*);
int GetRate(char*);
int GetChannles(char*);
int play(char*);

#define BUFSIZE 1024

#endif
