#!/bin/sh
gcc player.c -o libplayer.dylib -lpulse -lpulse-simple -lmagic -fPIC -shared "$@"
