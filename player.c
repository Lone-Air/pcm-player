/***
  This file is LMFS's extension of the PulseAudio demo program.
***/

/***
  This file is part of PulseAudio.
 
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.
 
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/

#include "player.h"

#ifdef __cplusplus
extern "C"{
#endif

bool islink(char* file_p){
	char buf[1024];
	ssize_t len;
	if((len=readlink(file_p, buf, 1024-1))!=-1){
		return true;
	}
	return false;
}

char* readlnk(char* file_p){
	char buf[1024];
	ssize_t len;
	if((len=readlink(file_p, buf, 1024-1))!=-1){
		buf[len]='\0';
	}
    char* link=(char*)malloc(sizeof(char)*strlen(buf));
    strcpy(link, buf);
	return link;
}

int IsNum(char c){
    return c>='0'&&c<='9';
}

int FullNum(char* c){
    int CH;
    while((CH=(*c++))!='\0'){
        if(!IsNum(CH)){
            return false;
        }
    }
    return true;
}

char* gettype(char* file){
    struct magic_set *cookie=NULL;
    cookie=magic_open(MAGIC_MIME_TYPE);
    magic_load(cookie, NULL);
    const char* text_D=magic_file(cookie,file);
    char* buf=(char*)malloc(sizeof(char)*strlen(text_D));
    strcpy(buf, text_D);
    magic_close(cookie);
    return buf;
}

Array split(char* str, const char* sp){
    char** list=(char**)malloc(sizeof(char*)*8);
    int maxmem=8;
    int nowmem=0;
    char* token;
    token=strtok(str, sp);
    while(token!=NULL){
        if(nowmem<maxmem){
            list[nowmem]=(char*)malloc(sizeof(char)*strlen(token));
            strcpy(list[nowmem++], token);
        }else{
            char** temp=(char**)malloc(sizeof(char*)*(maxmem=maxmem+8));
            for(int i=0;i<nowmem;i++){
                temp[i]=(char*)malloc(sizeof(char)*strlen(list[i]));
                strcpy(temp[i], list[i]);
            }
            free(list);
            list=temp;
            list[nowmem]=(char*)malloc(sizeof(char)*strlen(token));
            strcpy(list[nowmem++], token);
        }
        token=strtok(NULL, sp);
    }
    Array res={list, nowmem};
    return res;
}

int charpToInt(char* str){
    int res=0;
    char ch;
    for(int i=0;i<strlen(str);i++){
        ch=str[i];
        if(ch>='0'&&ch<='9')
          res=res*10+ch-48;
    }
    return res;
}

AudioInfo GetInfo(char* file){
    char* type=gettype(file);
    if(strcmp("audio/x-wav", type)!=0){
        fprintf(stderr, "player:error:not pcm file\n");
        return (AudioInfo){-1,-1};
    }
    magic_t cookie=magic_open(MAGIC_CONTINUE);
    magic_load(cookie, NULL);
    char text[1024];
    snprintf(text, 1024, "%s", magic_file(cookie, file));
    Array info=split(text, " ");
    int rate=-1;
    char* Channle;
    int channles=-1;
    for(int i=0;i<info.len;i++){
        int inter=info.len-i-1;
        if(FullNum(info.data[inter])){
            rate=charpToInt(info.data[inter]);
            Channle=info.data[inter-1];
            if(strcmp(Channle, "stereo")==0) channles=2;
            else if(strcmp(Channle, "mono")==0) channles=1;
            else fprintf(stderr,"player:error:Unable to get the number of channels to determine the audio file\n");
            break;
        }
    }
    magic_close(cookie);
    AudioInfo res={rate, channles};
    return res;
}

int play(char* file) {
 
    /* The Sample format to use */
    file=islink(file)?readlnk(file):file;
    AudioInfo Info=GetInfo(file);
    int RATE=Info.rate;
    int CHANNLES=Info.channles;
    if(CHANNLES==-1) return 0;
    if(RATE==-1) return 0;
    const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate=RATE,
        .channels = CHANNLES
    };
 
    pa_simple *s = NULL;
    int ret = 1;
    int error;
 
    /* replace STDIN with the specified file if needed */
//    if (argc > 1) {
        int fd;
 
        if ((fd = open(file, O_RDONLY)) < 0) {
            fprintf(stderr, __FILE__": open() failed: %s\n", strerror(errno));
            goto finish;
        }
 
        if (dup2(fd, STDIN_FILENO) < 0) {
            fprintf(stderr, __FILE__": dup2() failed: %s\n", strerror(errno));
            goto finish;
        }
 
        close(fd);
//    }
 
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, file, PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
 
    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;
 
#if 0
        pa_usec_t latency;
 
        if ((latency = pa_simple_get_latency(s, &error)) == (pa_usec_t) -1) {
            fprintf(stderr, __FILE__": pa_simple_get_latency() failed: %s\n", pa_strerror(error));
            goto finish;
        }
 
        fprintf(stderr, "%0.0f usec    \r", (float)latency);
#endif
 
        /* Read some data ... */
        if ((r = read(STDIN_FILENO, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* EOF */
                break;
 
            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            goto finish;
        }
 
        /* ... and play it */
        if (pa_simple_write(s, buf, (size_t) r, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    }
 
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }
 
    ret = 0;
 
finish:
 
    if (s)
        pa_simple_free(s);
 
    return ret;
}

void bgm(char* file){
    while(1)
      play(file);
}

int main(int argc, char* argv[]){
    if(argc<2){
        fprintf(stderr, "player:failed:no file input\n");
        return 0;
    }
    for(int i=1;i<argc;i++){
        printf("player:info:now play:%s",argv[i]);
        play(argv[i]);
    }
    return 0;
}


#ifdef __cplusplus
};
#endif
