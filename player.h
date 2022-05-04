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
    int rate;
    int channles;
    int bit;
}AudioInfo;

typedef struct{
    char** data;
    int len;
}Array;

static AudioInfo AudioTEMP={0,0,0};
static int Temped=0;

bool islink(char*);
char* readlnk(char*);
int IsNum(char);
int FullNum(char*);
char* gettype(char*);
Array split(char*, const char*);
int charpToInt(char*);
AudioInfo GetInfo(char*);
int play(char*);
int Play(char*, int, int, int);
void bgm(char*);
void Bgm(char*, int, int, int);
void initGet(char*);
int getrate();
int getchannles();
int getbit();
void eraseTMP();

#define BUFSIZE 1024

#endif
