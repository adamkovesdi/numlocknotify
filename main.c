#include <stdio.h>
#include <libnotify/notify.h>
#include <X11/Xlib.h>

int prevstate;

static void notify_numlockstate(const char* ns) {
	notify_init("Sample");
	NotifyNotification* n = notify_notification_new ("NumLock", 
			ns,
			0);
	notify_notification_set_timeout(n, 1500); // milliseconds
	if (!notify_notification_show(n, 0)) 
	{
		printf("show has failed\n");
	}
}

static int get_numlock_state(void) {
	Display *dpy = XOpenDisplay(":0"); 
	XKeyboardState x;
	XGetKeyboardControl(dpy, &x);
	XCloseDisplay(dpy);
	return (x.led_mask & 2) ? 1 : 0;
}

static void loop() {
		int ns = get_numlock_state();
		if(prevstate == ns) return;
		prevstate = ns;

		if(ns) {
			notify_numlockstate("on");
		}
		else {
			notify_numlockstate("off");
		}
		sleep(1);
}

int main(int argc, char * argv[]) {
	prevstate = get_numlock_state();
	while(1) loop();
	return 0;
}
