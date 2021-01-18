#!/bin/bash
gcc main.c -o numlockstate `pkg-config --cflags --libs libnotify` -lX11
