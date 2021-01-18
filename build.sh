#!/bin/bash
gcc main.c -o numlocknotify `pkg-config --cflags --libs libnotify` -lX11
