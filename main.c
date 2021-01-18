#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libnotify/notify.h>
#include <X11/Xlib.h>

int prevstate;

static void notify_numlockstate(const char* ns) {
	notify_init("Sample");
	NotifyNotification* n = notify_notification_new ("NumLock", 
			ns,
			0);
	notify_notification_set_timeout(n, 1500); // milliseconds
	if (!notify_notification_show(n, 0)) {
		// printf("show has failed\n");
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

static void spawn_daemon(void) {
	pid_t pid;
	/* Fork off the parent process */
	pid = fork();
	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);
	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);
	/* On success: The child process becomes session leader */
	if (setsid() < 0)
		exit(EXIT_FAILURE);
	/* Catch, ignore and handle signals */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	/* Fork off for the second time*/
	pid = fork();
	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);
	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);
	/* Set new file permissions */
	umask(0);
	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");
	/* Close all open file descriptors */
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
	{
		close (x);
	}
}

int main(int argc, char * argv[]) {
	prevstate = get_numlock_state();
	spawn_daemon();
	while(1) loop();
	return 0;
}
