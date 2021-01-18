#!/bin/bash
gcc numlocknotify.c -o numlocknotify -lX11 `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs libnotify` 
