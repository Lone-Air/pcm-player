#!/bin/sh
gcc player.c -o player.dll -lpulse -lpulse-simple -lmagic -fPIC -shared
