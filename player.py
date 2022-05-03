"""
    LMFS PCM Player (with libmagic, pulseaudio)
"""

import ctypes, os, signal

__version__="1.1.5"
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

def play(files):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    ID=ID+1
    if(pid==0):
        for i in files:
            try:
                dylib.play(i.encode())
            except: exit()
        exit()

def bgm(file):
    global pidL,pidS,ID
    pid=os.fork()
    pidL.append(pid)
    pidS.update({ID: pid})
    ID=ID+1
    if(pid==0):
        try:
            dylib.bgm(file.encode())
        except: exit()

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

