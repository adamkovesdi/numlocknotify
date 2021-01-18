# numlocknotify

One file GTK3 app for showing NumLock state through libnotify

Based on [indicator-keylock](https://code.launchpad.net/~tsbarnes/indicator-keylock)

## building

```
gcc numlocknotify.c -o numlocknotify -lX11 `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs libnotify`
```
