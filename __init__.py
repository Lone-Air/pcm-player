"""
    LMFS PCM Player
"""

import os

if(not os.path.exists(os.path.join(os.path.dirname(__file__),"libplayer.so"))):
    if(not os.path.exists(os.path.join(os.path.dirname(__file__),"libplayer.dylib"))):
        if(not os.path.exists(os.path.join(os.path.dirname(__file__),"player.dll"))):
            raise FileNotFoundError("You need run build_<your_platform>.sh to build the binary object")

from .player import *
