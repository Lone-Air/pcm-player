"""
    LMFS PCM Player (with libmagic, pulseaudio)
"""

import ctypes, os, signal

__version__="1.3"
pidL=[]
pidS={}
ID=0

if(os.name=="posix"):
    dylib=ctypes.CDLL(os.path.realpath(os.path.join(os.path.dirname(__file__),"libplayer.so")))
elif(os.name=="darwin"):
    dylib=ctypes.CDLL(os.path.realpath(os.path.join(os.path.dirname(__file__),"libplayer.dylib")))
elif(os.name=="nt"):
    dylib=ctypes.WinDLL(os.path.realpath(os.path.join(os.path.dirname(__file__),"player.dll")))
else:
    raise OSError("Your device is not recognized")

def play(files, wait=False):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    myid=ID
    ID=ID+1
    if(pid==0):
        for i in files:
            try:
                dylib.play(i.encode())
            except: exit()
        exit()
    elif(pid>0 and wait):
        while 1:
            wpid, status=os.waitpid(pid,0)
            if os.WIFEXITED(status) or os.WIFSIGNALED(status):
                delpid(myid)
                break

def playWithOpt(files, rate, channles, bit, wait=False):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    myid=ID
    ID=ID+1
    if(pid==0):
        for i in files:
            try:
                dylib.Play(i.encode(), rate, channles, bit)
            except: exit()
        exit()
    elif(pid>0 and wait):
        while 1:
            wpid, status=os.waitpid(pid,0)
            if os.WIFEXITED(status) or os.WIFSIGNALED(status):
                delpid(myid)
                break


def bgm(file, wait=False):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    myid=ID
    ID=ID+1
    if(pid==0):
        try:
            dylib.bgm(file.encode())
        except: exit()
        exit()
    elif(pid>0 and wait):
        while 1:
            wpid, status=os.waitpid(pid,0)
            if os.WIFEXITED(status) or os.WIFSIGNALED(status):
                delpid(myid)
                break


def bgmWithOpt(file, rate, channles, bit, wait=False):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    myid=ID
    ID=ID+1
    if(pid==0):
        try:
            dylib.Bgm(file.encode(), rate, channles, bit)
        except: exit()
        exit()
    elif(pid>0 and wait):
        while 1:
            wpid, status=os.waitpid(pid,0)
            if os.WIFEXITED(status) or os.WIFSIGNALED(status):
                delpid(myid)
                break

def initFile(file):
    dylib.initGet(file.encode())

getrate=lambda:dylib.getrate()
getchannles=lambda:dylib.getchannles()
getbit=lambda:dylib.getbit()

def eraseCache():
    dylib.eraseTMP()

def stop():
    global pidL,pidS,ID
    if(pidL==[]):
        raise OSError("No player was running")
    for i in pidL:
        os.kill(i, signal.SIGKILL)
    pidL=[]
    pidS={}
    ID=0

def stopById(Id):
    global pidL,pidS
    if(Id not in list(pidS.keys())):
        raise OSError("No player running in background was called id %d"%Id)
    pid=pidS[Id]
    os.kill(pid, signal.SIGKILL)
    delpid(Id)

def delpid(Id):
    global pidL,pidS,ID
    if(Id not in list(pidS.keys())):
        raise OSError("No player running in background was called id %d"%Id)
    pid=pidS[Id]
    del pidS[Id]
    del pidL[pidL.index(pid)]
    ID=ID-1
    organize()

def organize():
    global pidS
    pidSNew={}
    count=0
    for i in pidS.values():
        pidSNew[count]=i
        count+=1
    pidS=pidSNew.copy()

