#!/bin/sh
gcc player.c -o libplayer.so -lpulse -lpulse-simple -lmagic -fPIC -shared "$@"
